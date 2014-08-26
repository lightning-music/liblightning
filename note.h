#ifndef NOTE_H_INCLUDED
#define NOTE_H_INCLUDED

typedef struct Note *Note;

Note
Note_init(int number, int velocity, void *data);

int
Note_number(Note n);

int
Note_velocity(Note n);

void
Note_free(Note *note);

#endif
