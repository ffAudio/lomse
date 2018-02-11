/*

See:  https://github.com/0xfe/vexflow/wiki/The-VexFlow-Tutorial

For images: use LenMus at full screen and Zoom out six times

*/

﻿/**
@page page-im-modification The low level edition API

@tableofcontents



@section im-modification-intro  Low level edition API overview
<!---------------------------------------------------------------------------------->

When using the low level edition API your application will be interacting directly with the @IM. You should know what you are doing; otherwise, there is a risk of corrupting the @IM and causing crashes.

To get a quick idea in this section you will find some explained examples of different complexity for building and editing music scores and documents.

@subsection im-modification-intro-example-1   1. The basics

Let's create a music score from scratch:

@code
//create empty document
delete m_pPresenter;
m_pPresenter = m_lomse.new_document(ViewFactory::k_view_vertical_book);

//Add content to the document
if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
{
    //get the document to edit
    Document* pDoc = m_pPresenter->get_document_raw_ptr();

	//add an empty score to the document
    ImoScore* pScore = pDoc->add_score();

	//add an instrument (an score part) to the score
    ImoInstrument* pInstr = pScore->add_instrument();

	//add some content to this instrument
    pInstr->add_clef(k_clef_G2);			//G clef on second line
    pInstr->add_key_signature(k_key_D);		//D major key signature
    pInstr->add_time_signature(2, 4);		// 2/4 time signature
    pInstr->add_staff_objects("(n d5 q)(n a4 q)");	//two quarter notes D5 and A4
    pInstr->add_barline(k_barline_end);		//and a final barline

    //update the internal data structures
    pDoc->end_of_changes();
}
@endcode

Here's what you should get: 
<div class="left-aligned">
@image html im-modification-api-1.png  "Example 1: Score generated by previous code."
</div>

Notice that when we have finished modifying a document it is necessary to update all necessary internal data structures. For this, you must invoke `pDoc->end_of_changes()`. This method updates the internal data structures for each music score in the document. Alternatively, as we know that the only modified score is the one pointed by `pScore` we can save procesing time
by just invoking `pScore->end_of_changes()`.

Also notice that appart from specific methods for creating each object type, such as adding a clef by using method `add_clef()` or `add_time_signature()`, it is also posible to use generic methods that takes as parameter the LDP source code for the objects to add, as we did for adding the notes. Here you have an alternative to previous example code:

@code
	ImoInstrument* instr = pScore->add_instrument();
	instr->add_object( "(clef G2)" );                    
	instr->add_object( "(key D)" );
	instr->add_object( "(time 2 4)" );
	instr->add_object( "(n d4 q)" );
	instr->add_object( "(n a4 q)" );
	instr->add_object( "(barline end)" );
    pScore->end_of_changes();
@endcode



@subsection im-modification-intro-example-2   2. Editing an existing document
<!---------------------------------------------------------------------------------->

In this second example, lets edit an existing document. It is irrelevant how the Document was created. So, lets use the document created in previous example, and lets add a second voice with a single half note to this tune:

@code
if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
{
    //get the document to edit
    Document* pDoc = m_pPresenter->get_document_raw_ptr();

    //Use the document to get the score to edit
    //The next line of code is just an example, in which it is assumed that
    //the score to edit is the first element in the document, as it is the case if we
	//use the document created in previous example. Also, this will be always the
    //case when editing MusicXML imported files.
    ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );

    if (pScore)
    {
        //get the instrument (score part) to be edited. In this example, it is
        //the first instrument. 
        ImoInstrument* pInstr = pScore->get_instrument(0);

        //for inserting notes or other objects it is necessary to determine the 
        //insertion point. For locating the insertion point there are
        //many possibilities, usually involving an iterator or a cursor object.
        //Lets use a cursor object:
        ScoreCursor cursor(pDoc, pScore);

        //after creation the cursor will be pointing to the first object in
        //the score, the clef in this example
        cursor.move_next();		//now points to key signature
        cursor.move_next();		//now points to time signature
        cursor.move_next();		//done! now points to the first note, the D5 quarter note
        ImoStaffObj* pAt = cursor.staffobj();        //get the pointed object: note D5

        //change stem of note D5 to go up
        ImoNote* pNote = static_cast<ImoNote*>(pAt);
        pNote->set_stem_direction(k_stem_up);

        //insert a half D4 note in voice 2, with stem down
        stringstream errormsg;
        if (!pInstr->insert_staffobj_at(pAt, "(n d4 h v2 (stem down))", errormsg))
        {
            //error. An explanation is in errormsg
        }

        //once the updates are finished, invoke close() method for
        //for updating the internal data structures. This equivalent to
		//invoking pDoc->end_of_changes(); but will only rebuild
		//the structures associated to the modified score
        pScore->close();      
    }
}
@endcode

Here's what you should get: 
<div class="left-aligned">
@image html im-modification-api-2.png  "Example 2: Score generated by previous code."
</div>




@subsection im-modification-intro-example-3   3. Different ways for creating notes
<!----------------------------------------------------------------------------------------------------->

In previous examples the notes to add were created using LDP source code. In this example a different
way for creating notes is shown. It demonstrates the right way for creating nodes before adding them to
the internal model. This is required for using some low level API methods.


@code
//create empty document
delete m_pPresenter;
m_pPresenter = m_lomse.new_document(ViewFactory::k_view_vertical_book);

//Add content to the document
if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
{
    //get the document to edit
    Document* pDoc = m_pPresenter->get_document_raw_ptr();

	//add an empty score to the document
    ImoScore* pScore = pDoc->add_score();

	//add an instrument (an score part) to the score
    ImoInstrument* pInstr = pScore->add_instrument();

	//add some content to this instrument
    pInstr->add_clef(k_clef_G2);			//G clef on second line
    pInstr->add_key_signature(k_key_D);		//D major key signature
    pInstr->add_time_signature(2, 4);		// 2/4 time signature

    //create the first note to insert: D4 half note
    //The following code shows the right way for creating nodes before
    //adding them to the model by using the ImFactory object (in lomse_im_factory.h).
    ImoNote* pNote = static_cast<ImoNote*>( ImFactory::inject(k_imo_note, pDoc) );
    pNote->set_note_type_and_dots(k_half, 0);
    pNote->set_voice(1);
    pNote->set_notated_pitch(k_step_D, 4, k_no_accidentals);
    pInstr->insert_staffobj_at(nullptr, pNote);		//append at end

    pInstr->add_object("(r h)");            //add a rest, duration: half
    pInstr->add_barline(k_barline_simple);  //add barline to finish first measure
    pInstr->add_object("(n f4 q.)");        //add note: flat F4 dotted quarter note
    pInstr->add_object("(n a4 e)");         //add the third note: A4 8th note

    //update the internal data structures
    pScore->end_of_changes();
}
@endcode

Here's what you should get: 
<div class="left-aligned">
@image html im-modification-api-3.png  "Example 3: Score generated by previous code."
</div>

Notice that we didn't add a barline at the end. Therefore the last measure is not finished and the staff lines run until the right margin of the page.


@todo Finish the low level edition API overview

*/
