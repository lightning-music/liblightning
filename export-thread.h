
#include "types.h"

typedef struct ExportThread *ExportThread;

/**
 * client code must send the Continue signal as the
 * value of the data event for the thread to write data
 * from the ringbuffer to the file.
 * when the thread receives the Stop signal it will stop writing
 * and close the file
 */
typedef enum {
    ExportThread_Idle = 100,
    ExportThread_Continue = 200,
    ExportThread_Stop = 300
} ExportThread_Signal;

/**
 * Start a thread for writing to a file
 *
 * @return ExportThread structure
 */
ExportThread
ExportThread_create(const char *file, nframes_t output_sr, channels_t channels);

/**
 * Make the export thread wait for data
 *
 * @param ExportThread structure
 */
nframes_t
ExportThread_write(ExportThread thread, sample_t **bufs, nframes_t frames);

/**
 * Signal that there is data, with an optional signal to tell the
 * thread to stop.
 */
int
ExportThread_signal(ExportThread thread, ExportThread_Signal *signal);

/**
 * Destroy an export thread
 */
void
ExportThread_free(ExportThread *thread);

