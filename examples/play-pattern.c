
#include "../lightning.h"
#include "../note.h"
#include "../pattern.h"

int main(int argc, char **argv)
{
    Lightning lightning = Lightning_init(120); // bpm
    Pattern p = Pattern_init(5, "audio/snap.flac");

    Pattern_append_note(p, Note_init(60, 120));
    Pattern_append_note(p, Note_init(62, 120));
    Pattern_append_note(p, Note_init(64, 120));
    Pattern_append_note(p, Note_init(65, 120));
    Pattern_append_note(p, Note_init(67, 120));

    Pattern_free(&p);
    Lightning_free(&lightning);

    return 0;
}
