//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Lomse is copyrighted work (c) 2010-2016. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#ifndef __LOMSE_MIDI_TABLE_H__        //to avoid nested includes
#define __LOMSE_MIDI_TABLE_H__

#include "lomse_pitch.h"
#include "lomse_time.h"

#include <vector>
#include <string>
using namespace std;


namespace lomse
{

//forward declarations
class ImoScore;
class ImoInstrument;
class ImoStaffObj;
class ImoKeySignature;
class ImoTimeSignature;
class ImoNote;
class StaffObjsCursor;
class SoundEvent;
class SoundEventsTable;

//---------------------------------------------------------------------------------------
//JumpsEntry: An entry in the JumpsTable. It describes a jump in playback.
class JumpEntry
{
protected:
	bool m_fIsActive;           //true if this jump is still valid
	int m_measure;		        //number of the measure to jump to
	int m_numTimes;		        //num.times the jumpTo must be active
	int m_applied;		        //num.times the jumpTo has been executed
	SoundEvent* m_event;        //ptr to event to jump to

public:

    JumpEntry(int jumpTo, int times);
    virtual ~JumpEntry();

//	void reset_entry();
//	int exec_jump();

	inline bool is_active() { return m_fIsActive; }

	inline int get_measure() { return m_measure; }
	inline int get_times() { return m_numTimes; }
	inline int get_applied() { return m_applied; }
	inline SoundEvent* get_event() { return m_event; }

    inline void set_event(SoundEvent* pSE) { m_event = pSE; }
    inline void set_measure(int measure) { m_measure = measure; }
    inline void set_times(int times) { m_numTimes = times; }


    //debug
    string dump_entry(SoundEventsTable* pMidiTable);

};

//---------------------------------------------------------------------------------------
//auxiliary class SoundEvent describes a sound event
class SoundEvent
{
public:
    SoundEvent(TimeUnits rTime, int nEventType, int nChannel,
               MidiPitch midiPitch, int nVolume, int nStep,
               ImoStaffObj* pStaffObj, int nMeasure)
        : DeltaTime(long(rTime + 0.5f))
        , EventType(nEventType)
        , Channel(nChannel)
        , NotePitch(midiPitch)
        , Volume(nVolume)
        , NoteStep(nStep)
        , pSO(pStaffObj)
        , Measure(nMeasure)
    {
    }
    SoundEvent(TimeUnits rTime, int nEventType, JumpEntry* pJumpEntry, int nMeasure)
        : DeltaTime(long(rTime + 0.5f))
        , EventType(nEventType)
        , Channel(0)
        , NotePitch(0)
        , Volume(0)
        , NoteStep(0)
        , pJump(pJumpEntry)
        , Measure(nMeasure)
    {
    }
    ~SoundEvent() {}

    enum
    {
        // AWARE Event type value is used to sort the events table.
        // MUST keep order for priority
        k_prog_instr = 1,       //program a new sound
        k_note_off,             //sound off
        k_visual_off,           //remove visual highlight. No effect on sound
        k_rhythm_change,        //change in rhythm (time signature)
        k_jump,                 //jump in playback (repetition mark, volta bracket,...)
        k_note_on,              //sound on
        k_visual_on,            //add visual highlight. No effect on sound
        k_end_of_score,         //end of table
    };

    long        DeltaTime;      //Relative to metronome speed
    int         EventType;
    int         Channel;
    union {
        int     NotePitch;      //k_note_xxx: MIDI pitch
        int     Instrument;     //k_prog_instr: MIDI instrument
        int     NumPulses;      //k_rhythm_change: num. metronome pulses per measure
    };
    union {
        int     Volume;             //k_note_xxx: for notes
        int     MeasureDuration;    //k_rhythm_change: In LDP duration units
    };
    int             NoteStep;   //k_note_xxx: Note step 0..6 : 0-Do, ... 6-Si
    union {
        ImoStaffObj*    pSO;        //staffobj who originated the event (for visual highlight)
        JumpEntry*      pJump;      //jump entry, for playback jumps
    };
    int             Measure;    //measure number containing this staffobj

};

//---------------------------------------------------------------------------------------
//Class SoundEventsTable stores and manages all sound events related to a score
class SoundEventsTable
{
protected:
    ImoScore* m_pScore;
    int m_numMeasures;
    vector<SoundEvent*> m_events;
    vector<int> m_measures;
    vector<int> m_channels;
    vector<JumpEntry*> m_jumps;
    TimeUnits rAnacrusisMissingTime;
    int m_accidentals[7];

public:
    SoundEventsTable(ImoScore* pScore);
    virtual ~SoundEventsTable();

    void create_table();

    inline int num_events() { return int(m_events.size()); }
    vector<SoundEvent*>& get_events() { return m_events; }
    vector<int>& get_channels() { return m_channels; }
    inline int get_first_event_for_measure(int nMeasure) { return m_measures[nMeasure]; }
    inline int get_last_event() { return int(m_events.size()) - 1; }
    inline int get_num_measures() { return m_numMeasures; }
    inline TimeUnits get_anacrusis_missing_time() { return rAnacrusisMissingTime; }
    inline bool is_anacrusis_start() { return is_greater_time(rAnacrusisMissingTime, 0.0f); }

    //jumps table
    inline int num_jumps() { return int(m_jumps.size()); }
    JumpEntry* get_jump(int i);

    //debug
    string dump_midi_events();
    int get_event_index(SoundEvent* pSE);


protected:
    void store_event(TimeUnits rTime, int eventType, int channel, MidiPitch pitch,
                     int volume, int step, ImoStaffObj* pSO, int measure);
    void store_jump_event(TimeUnits rTime, JumpEntry* pJump, int measure);
    void program_sounds_for_instruments();
    void create_events();
    void close_table();
    void sort_by_time();
    void create_measures_table();
    void add_jumps_if_volta_bracket(StaffObjsCursor& cursor, ImoBarline* pBar,
                                    int measure);
    void add_events_to_jumps();
    void add_noterest_events(StaffObjsCursor& cursor, int channel, int measure);
    void add_rythm_change(StaffObjsCursor& cursor, int measure, ImoTimeSignature* pTS);
    void add_jump(StaffObjsCursor& cursor, int measure, JumpEntry* pJump);
    void delete_events_table();
    int compute_volume(TimeUnits timePos, ImoTimeSignature* pTS);
    void reset_accidentals(ImoKeySignature* pKey);
    void update_context_accidentals(ImoNote* pNote);
    JumpEntry* create_jump(int jumpTo, int times);


    //debug
    string dump_events_table();
    string dump_measures_table();
};


}   //namespace lomse

#endif  // __LOMSE_MIDI_TABLE_H__
