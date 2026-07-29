/**
 * cvfs.h - public interface for the Customized Virtual File System (CVFS)
 *
 * declares all filesystem limits, error codes, on-disk/in-memory
 * structures, and the function prototypes implemented in cvfs.c.
 * kept in a separate header so the data structures and API surface
 * can be understood without reading the implementation.
 */

#ifndef CVFS_H
#define CVFS_H

#include <stdbool.h>

/* Filesystem limits */
#define MAX_FILE_SIZE   50
#define MAX_OPEN_FILES  20
#define MAX_INODES      5

/* File access permission flags */
#define READ            1
#define WRITE           2
#define EXECUTE         4

/* File seek positions */
#define START           0
#define CURRENT         1
#define END             2

#define EXECUTE_SUCCESS 0

#define REGULAR_FILE    1
#define SPECIAL_FILE    2

/* User defined macros for error handling */
#define ERR_INVALID_PARAMETER   -1
#define ERR_NO_INODES           -2
#define ERR_FILE_ALREADY_EXISTS -3
#define ERR_FILE_NOT_EXISTS     -4
#define ERR_PERMISSION_DENIED   -5
#define ERR_INSUFFICIENT_SPACE  -6
#define ERR_INSUFFICIENT_DATA   -7
#define ERR_MAX_FILES_OPEN      -8

/**
 * struct BootBlock - boot time filesystem information
 *
 * @information: textual boot information
 *
 * acts as a placeholder for future boot-related metadata.
 */
struct BootBlock
{
	char information[100];
};

/**
 * struct SuperBlock - global filesystem metadata
 *
 * @total_inodes: total number of supported inodes
 * @free_inodes: number of currently free inodes
 *
 * maintains overall state of the virtual filesystem.
 */
struct SuperBlock
{
	int total_inodes;
	int free_inodes;
};

/**
 * struct Inode - in-memory representation of a file
 *
 * @file_name:        name of the file
 * @buffer:           pointer to file data
 * @inode_number:     unique inode identifier
 * @file_size:        maximum allowed size
 * @file_type:        REGULAR_FILE / SPECIAL_FILE / 0 (free)
 * @actual_file_size: bytes currently written
 * @reference_count:  number of active references
 * @permission:       access permission mask
 * @next:             next inode in disk inode list block
 *
 * represents file metadata inside cvfs.
 */
#pragma pack(1)
struct Inode
{
	char file_name[20];
	char *buffer;
	int  inode_number;
	int  file_size;
	int  file_type;
	int  actual_file_size;
	int  reference_count;
	int  permission;
	struct Inode *next;
};

typedef struct Inode INODE;
typedef struct Inode *PINODE;
typedef struct Inode **PPINODE;

/**
 * struct FileTable - open file instance
 *
 * @read_offset: current read position
 * @write_offset: current write position
 * @mode: access mode
 * @ptr_inode: associated inode
 *
 * each open() call creates a file table entry.
 */
struct FileTable
{
	int read_offset;
	int write_offset;
	int mode;
	PINODE ptr_inode;
};

typedef struct FileTable FILE_TABLE;
typedef struct FileTable *PFILE_TABLE;

/**
 * struct UAREA - process-specific file descriptor table
 *
 * @process_name: name of running process
 * @UFDT: user file descriptor table
 *
 * holds per-process open file information.
 */
struct UAREA
{
	char process_name[20];
	PFILE_TABLE UFDT[MAX_OPEN_FILES];
};

/* ---------------------------------------------------------------- *
 |                     Initialisation routines                      |
 * ---------------------------------------------------------------- */

void initialise_uarea(void);
void initialise_super_block(void);
void create_DILB(void);
void start_auxillary_data_initialisation(void);

/* ---------------------------------------------------------------- *
 |                        Shell / help routines                     |
 * ---------------------------------------------------------------- */

void display_help(void);
void man_page_display(char command[]);

/* ---------------------------------------------------------------- *
 |                      Core filesystem operations                  |
 * ---------------------------------------------------------------- */

bool   is_file_exists(char *name);
PINODE get_inode_of_file(char *name);

int  create_file(char *name, int permission);
int  write_file(int fd, char *data);
int  read_file(int fd, int size);
int  stat_file(char *name);
int  unlink_file(char *name);
void ls_file(void);

#endif /* CVFS_H */
