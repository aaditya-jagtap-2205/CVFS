/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "../include/cvfs.h"

/* Global filesystem objects */
struct BootBlock  boot_obj;
struct SuperBlock super_obj;
struct UAREA      uarea_obj;

/* Head of disk inode list block */
PINODE head = NULL;

/**
 * initialise_uarea - initialize user area
 *
 * clears all file descriptor entries and sets
 * the name of the current process.
 */
void initialise_uarea()
{
	strcpy(uarea_obj.process_name, "Myexe");	
	
	for(int i = 0; i < MAX_OPEN_FILES; ++i)
	{
		uarea_obj.UFDT[i] = NULL;
	}
	printf("CVFS: UAREA gets initialised succesfully\n");
}

/**
 * initialise_super_block - initialize superblock metadata
 */
void initialise_super_block()
{
	super_obj.total_inodes = MAX_INODES;
	super_obj.free_inodes  = MAX_INODES;

	printf("CVFS : Super block gets initialised succesfully\n");
}

/**
 * create_DILB - create disk inode list block
 *
 * builds a linked list of free inodes at startup.
 */
void create_DILB()
{
	PINODE new_n = NULL;
	PINODE temp  = head;

	for(int i = 1; i <= MAX_INODES; i++)
	{
		new_n = (PINODE)malloc(sizeof(INODE));		

		strcpy(new_n->file_name, "");

		new_n->inode_number     = i;
		new_n->file_size        = 0;
		new_n->file_type        = 0;
		new_n->actual_file_size = 0;
		new_n->reference_count  = 0;
		new_n->buffer           = NULL;
		new_n->next             = NULL;

		if(temp == NULL)
		{
			head = new_n;
			temp = head;
		}
		else
		{
			temp->next = new_n;
			temp = temp->next;
		}
	}
	printf("CVFS: DILB created sucsesfully\n");
}

/**
 * start_auxillary_data_initialisation - initialize cvfs core
 *
 * initialization order:
 *  superblock -> inode list -> uarea
 */
void start_auxillary_data_initialisation()
{
	strcpy(boot_obj.information, "Booting the CVFS...");
	printf("%s\n", boot_obj.information);
	
	initialise_super_block();
	create_DILB();
	initialise_uarea();

	printf("CVFS auxillary data initialised succesfully\n");
}

/**
 * display_help - it is used to display help
 *
 * upon user input as `help` this function displays
 * the help page similar to OS help page
 */
void display_help(void)
{
	printf("-----------------------------------------------------------------\n");
	printf("----------------------- CVFS Help Page --------------------------\n");
	printf("-----------------------------------------------------------------\n");

	printf("man   : It is used to display manual page\n");
	printf("clear : It is used to clear the CVFS terminal\n");
	printf("ls    : It is used to list names of all files\n");
	printf("creat : It is used to create a new file\n");
	printf("write : It is used to write data to a file\n");
	printf("read  : It is used to read data from a file\n");
	printf("stat  : It is used to print metadata about a file\n");
	printf("unlink: It is used to delete a file\n");
	printf("exit  : It is used to terminate CVFS\n");

	printf("-----------------------------------------------------------------\n");
}

/**
 * man_page_display - displays the man page for input command
 *
 * @command : the command name for man page
 *
 * it prints the manual page for particular commands and this
 * function will handle the command inputs
 */
void man_page_display(char command[])
{
	if(strcmp("ls", command) == 0)
	{
		printf("About : It is used to list names of all files\n");
		printf("Usage : ls\n");
	}
	else if(strcmp("man", command) == 0)
	{
		printf("About : It is used to display manual page\n");
		printf("Usage : man command_name\n");
		printf("command_name : It is the name of command\n");
	}
	else if(strcmp("exit", command) == 0)
	{
		printf("About : It is used to terminate the current process i.e., terminal\n");
		printf("Usage : exit\n");
	}
	else if(strcmp("clear", command) == 0)
	{
		printf("About : It is used to clear the shell\n");
		printf("Usage : clear\n");
	}
	else if(strcmp("creat", command) == 0)
	{
		printf("About : It is used to create a new regular file\n");
		printf("Usage : creat file_name permission\n");
		printf("permission : 1 - READ, 2 - WRITE, 3 - READ + WRITE\n");
	}
	else if(strcmp("write", command) == 0)
	{
		printf("About : It is used to write data into an existing file\n");
		printf("Usage : write fd data\n");
	}
	else if(strcmp("read", command) == 0)
	{
		printf("About : It is used to read data from an existing file\n");
		printf("Usage : read fd size\n");
	}
	else if(strcmp("stat", command) == 0)
	{
		printf("About : It is used to display metadata of a file\n");
		printf("Usage : stat file_name\n");
	}
	else if(strcmp("unlink", command) == 0)
	{
		printf("About : It is used to delete an existing file\n");
		printf("Usage : unlink file_name\n");
	}
	else
	{
		printf("No manual entry for %s\n", command);
	}
}

/**
 * is_file_exists - it is used to check whelther file is already in existence
 *
 * @name : filename
 *
 * it's a boolean function which returns true or false
 *
 * Return:
 * True or False
 */
bool is_file_exists(char *name)
{
	PINODE temp = head;
	bool b_flag = false;

	while(temp != NULL)
	{
		if((strcmp(name, temp->file_name) == 0) && (temp->file_type == REGULAR_FILE))
		{
			/* file exists */
			b_flag = true;
			break;
		}
		temp = temp->next;
	}
	return b_flag;
}

/**
 * get_inode_of_file - finds the inode belonging to a given file name
 *
 * @name : filename
 *
 * walks the disk inode list block and returns the matching inode.
 * caller is expected to check is_file_exists() first.
 *
 * Return:
 * pointer to matching inode, or NULL if not found
 */
PINODE get_inode_of_file(char *name)
{
	PINODE temp = head;

	while(temp != NULL)
	{
		if((strcmp(name, temp->file_name) == 0) && (temp->file_type == REGULAR_FILE))
		{
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

/**
 * create_file - creates a new regular file
 *
 * @name       : name of new file
 * @permission : permission for that file(READ/WRITE/EXECUTE)
 *
 * it accepts filename and permissions and it returns
 * the file descriptor
 */
int create_file(char *name, int permission)
{
	/* Create a temp instead of using HEAD directly */
	PINODE temp = head;
	int i = 0;

	printf("Total number of inodes remaining: %d\n", super_obj.free_inodes);
	
	/* If name of file is missing */
	if(name == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}
	/* If the permission value is incorrect 
	   permission -> 1 -> READ
	   permission -> 2 -> WRITE
	   permission -> 3 -> READ + WRITE */
	if(permission < 1 || permission > 3)
	{
		return ERR_INVALID_PARAMETER;
	}
	/* If there are no free inodes */
	if(super_obj.free_inodes == 0)
	{
		return ERR_NO_INODES;
	}

	/*------------------------------------*
	 |      Atleast one inode exists      |    
	 *------------------------------------*/
	/* If file already existing */
	if(is_file_exists(name) == true)
	{
		return ERR_FILE_ALREADY_EXISTS;
	}

	/* Check if any free inode */
	while(temp != NULL)
	{
		if(temp->file_type == 0)
		{
			break;
		}
		temp = temp->next;
	}
	
	if(temp == NULL)
	{
		printf("ERROR: There is no inode");
		return ERR_NO_INODES;
	}

	/* Search for empty UFDT entry */
	/* NOTE: 0,1,2 are reserved */
	for(i = 3; i < MAX_OPEN_FILES; ++i)
	{
		/* Stop at the first NULL index */
		if(uarea_obj.UFDT[i] == NULL)
		{
			break;
		}
		/* UFDT is full */
		if(i == MAX_OPEN_FILES)
		{
			return ERR_MAX_FILES_OPEN;
		}
	}

	/* Allocate memory for file table */
	uarea_obj.UFDT[i] = (PFILE_TABLE)malloc(sizeof(FILE_TABLE));

	/* Initialise file table */
	uarea_obj.UFDT[i] -> read_offset  = 0;
	uarea_obj.UFDT[i] -> write_offset = 0;
	uarea_obj.UFDT[i] -> mode         = permission;

	/* Connect file table with inode */
	uarea_obj.UFDT[i] -> ptr_inode = temp;

	/* Initialise members of inode */
	strcpy(uarea_obj.UFDT[i] -> ptr_inode -> file_name,  name);
	uarea_obj.UFDT[i] -> ptr_inode -> file_size        =  MAX_FILE_SIZE;
	uarea_obj.UFDT[i] -> ptr_inode -> actual_file_size = 0;
	uarea_obj.UFDT[i] -> ptr_inode -> file_type        = REGULAR_FILE;
	uarea_obj.UFDT[i] -> ptr_inode -> reference_count  = 1;
	uarea_obj.UFDT[i] -> ptr_inode -> permission       = permission;

	/* Allocate memory for files data */
	uarea_obj.UFDT[i] -> ptr_inode -> buffer = (char *)malloc(MAX_FILE_SIZE);

	super_obj.free_inodes--;

	return i;
}

/**
 * write_file - writes data into an already created file
 *
 * @fd   : file descriptor returned by creat
 * @data : null terminated data to be written
 *
 * appends data to the file's buffer starting at the current
 * write_offset, provided the file was opened with WRITE
 * permission and there is enough space left in the file.
 *
 * Return:
 * number of bytes written on success, else a negative error code
 */
int write_file(int fd, char *data)
{
	int data_size = 0;
	int remaining_space = 0;
	PFILE_TABLE fd_obj = NULL;

	if(fd < 3 || fd >= MAX_OPEN_FILES || data == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}
	if(uarea_obj.UFDT[fd] == NULL)
	{
		return ERR_FILE_NOT_EXISTS;
	}

	fd_obj = uarea_obj.UFDT[fd];

	/* mode 2 -> WRITE, mode 3 -> READ + WRITE */
	if(fd_obj->mode != 2 && fd_obj->mode != 3)
	{
		return ERR_PERMISSION_DENIED;
	}

	data_size = strlen(data);
	remaining_space = fd_obj->ptr_inode->file_size - fd_obj->ptr_inode->actual_file_size;

	if(data_size > remaining_space)
	{
		return ERR_INSUFFICIENT_SPACE;
	}

	/* Copy the incoming data right after previously written data */
	memcpy(fd_obj->ptr_inode->buffer + fd_obj->write_offset, data, data_size);

	fd_obj->write_offset               += data_size;
	fd_obj->ptr_inode->actual_file_size += data_size;

	return data_size;
}

/**
 * read_file - reads data from an already created file
 *
 * @fd   : file descriptor returned by creat
 * @size : number of bytes requested by the caller
 *
 * reads at most `size` bytes starting from the current
 * read_offset, provided the file was opened with READ
 * permission. prints the data that was read.
 *
 * Return:
 * number of bytes actually read on success, else a negative error code
 */
int read_file(int fd, int size)
{
	int available   = 0;
	int read_size   = 0;
	PFILE_TABLE fd_obj = NULL;
	char temp_buffer[MAX_FILE_SIZE + 1] = {'\0'};

	if(fd < 3 || fd >= MAX_OPEN_FILES || size <= 0)
	{
		return ERR_INVALID_PARAMETER;
	}
	if(uarea_obj.UFDT[fd] == NULL)
	{
		return ERR_FILE_NOT_EXISTS;
	}

	fd_obj = uarea_obj.UFDT[fd];

	/* mode 1 -> READ, mode 3 -> READ + WRITE */
	if(fd_obj->mode != 1 && fd_obj->mode != 3)
	{
		return ERR_PERMISSION_DENIED;
	}

	available = fd_obj->ptr_inode->actual_file_size - fd_obj->read_offset;

	if(available <= 0)
	{
		return ERR_INSUFFICIENT_DATA;
	}

	/* Do not read past whatever has actually been written */
	read_size = (size < available) ? size : available;

	memcpy(temp_buffer, fd_obj->ptr_inode->buffer + fd_obj->read_offset, read_size);
	temp_buffer[read_size] = '\0';

	printf("Data read from file: %s\n", temp_buffer);

	fd_obj->read_offset += read_size;

	return read_size;
}

/**
 * stat_file - displays metadata about a file
 *
 * @name : name of the file
 *
 * looks up the inode by file name and prints out its
 * metadata such as inode number, size and permission.
 *
 * Return:
 * EXECUTE_SUCCESS on success, else a negative error code
 */
int stat_file(char *name)
{
	PINODE temp = NULL;

	if(name == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}
	if(is_file_exists(name) == false)
	{
		return ERR_FILE_NOT_EXISTS;
	}

	temp = get_inode_of_file(name);

	printf("-----------------------------------------------------------------\n");
	printf("Inode Number     : %d\n", temp->inode_number);
	printf("File Name        : %s\n", temp->file_name);
	printf("File Size        : %d\n", temp->file_size);
	printf("Actual File Size : %d\n", temp->actual_file_size);
	printf("Permission       : %d\n", temp->permission);
	printf("Reference Count  : %d\n", temp->reference_count);
	printf("-----------------------------------------------------------------\n");

	return EXECUTE_SUCCESS;
}

/**
 * unlink_file - deletes an existing file
 *
 * @name : name of the file to delete
 *
 * releases the file's data buffer, closes any open file table
 * entries pointing to it, resets the inode back to a free
 * inode, and returns it to the free inode pool.
 *
 * Return:
 * EXECUTE_SUCCESS on success, else a negative error code
 */
int unlink_file(char *name)
{
	PINODE temp = NULL;

	if(name == NULL)
	{
		return ERR_INVALID_PARAMETER;
	}
	if(is_file_exists(name) == false)
	{
		return ERR_FILE_NOT_EXISTS;
	}

	temp = get_inode_of_file(name);

	/* Close every open file table entry that refers to this inode */
	for(int i = 3; i < MAX_OPEN_FILES; ++i)
	{
		if(uarea_obj.UFDT[i] != NULL && uarea_obj.UFDT[i]->ptr_inode == temp)
		{
			free(uarea_obj.UFDT[i]);
			uarea_obj.UFDT[i] = NULL;
		}
	}

	/* Release the file's data and reset the inode to "free" state */
	free(temp->buffer);
	temp->buffer = NULL;

	strcpy(temp->file_name, "");
	temp->file_size        = 0;
	temp->actual_file_size = 0;
	temp->file_type        = 0;
	temp->reference_count  = 0;
	temp->permission       = 0;

	super_obj.free_inodes++;

	return EXECUTE_SUCCESS;
}

/**
 * ls_file - it is used to list all files
 *
 * it displays list of all file on stdout
 */
void ls_file()
{
	PINODE temp = head;

	printf("-----------------------------------------------------------------\n");
	printf("--------------------- CVFS files information --------------------\n");
	printf("-----------------------------------------------------------------\n");
	while(temp != NULL)
	{
		if(temp->file_type != 0)
		{
			printf("%d\t%s\t%d\n", temp->inode_number, temp->file_name, temp->actual_file_size);
		}
		temp = temp->next;
	}
	printf("-----------------------------------------------------------------\n");
}

/* Entry point function */
int main()
{
	char str[80] = {'\0'};
	char command[5][20] = {{'\0'}};
	int  iCount = 0, iRet = 0;

	start_auxillary_data_initialisation();	

	printf("-----------------------------------------------------------------\n");
	printf("--------------------- CVFS started successfully------------------\n");
	printf("-----------------------------------------------------------------\n");

	/* Infinite shell */
	while(1)
	{
		fflush(stdin);

		strcpy(str, "");
		
		printf("\nCVFS: > ");
		fgets(str, sizeof(str), stdin);

		iCount = sscanf(str, "%s %s %s %s %s", command[0], command[1], command[2], command[3], command[4]);
		fflush(stdin);

		if(iCount == 1)
		{
			/* CVFS > exit */
			if(strcmp("exit", command[0]) == 0)
			{
				printf("Thank you for using CVFS\n");
				printf("De-allocating all the allocated resources\n");
				break;
			}
			/* CVFS > ls */
			else if(strcmp("ls", command[0]) == 0)
			{
				ls_file();
			}
			/* CVFS > help */
			else if(strcmp("help", command[0]) == 0)
			{
				display_help();
			}
			/* CVFS > clear */
			else if(strcmp("clear", command[0]) == 0)
			{
					#ifdef _WIN32
						system("cls");
					#else
						system("clear");
					#endif

			}
			else
			{
				printf("Command not found\n");
				printf("Please refer help option to get more information\n");
			}
		}
		else if(iCount == 2)
		{
			/* CVFS > man command_name */
			if(strcmp("man", command[0]) == 0)
			{
				man_page_display(command[1]);
			}
			/* CVFS > stat file_name */
			else if(strcmp("stat", command[0]) == 0)
			{
				iRet = stat_file(command[1]);
				if(iRet == ERR_INVALID_PARAMETER)
				{
					printf("ERROR: Invalid parameters. Please refer man page\n");
				}
				if(iRet == ERR_FILE_NOT_EXISTS)
				{
					printf("ERROR: File does not exist\n");
				}
			}
			/* CVFS > unlink file_name */
			else if(strcmp("unlink", command[0]) == 0)
			{
				iRet = unlink_file(command[1]);
				if(iRet == ERR_INVALID_PARAMETER)
				{
					printf("ERROR: Invalid parameters. Please refer man page\n");
				}
				if(iRet == ERR_FILE_NOT_EXISTS)
				{
					printf("ERROR: File does not exist\n");
				}
				if(iRet == EXECUTE_SUCCESS)
				{
					printf("File deleted successfully\n");
				}
			}
			else
			{
				printf("Command not found\n");
				printf("Please refer help option to get more information\n");
			}
		} /* End of else if 2*/
		else if(iCount == 3)
		{
			/* CVFS > creat LSP.txt 3 */
			if(strcmp("creat", command[0]) == 0)
			{
				iRet = create_file(command[1], atoi(command[2]));
				if(iRet == ERR_INVALID_PARAMETER)
				{
					printf("ERROR: Invalid paramters. Please refer man page\n");

				}
				if(iRet == ERR_NO_INODES)
				{
					printf("ERROR: No space to create a file\n");
				}
				if(iRet == ERR_FILE_ALREADY_EXISTS)
				{
					printf("ERROR: File already exists\n");
				}
				if(iRet == ERR_MAX_FILES_OPEN)
				{
					printf("ERROR: Can't access the file as many files open currently\n");
				}
				if(iRet >= 3)
				{
					printf("File gets succesfully created with fd: %d\n", iRet);
				}
			}
			/* CVFS > write fd data */
			else if(strcmp("write", command[0]) == 0)
			{
				iRet = write_file(atoi(command[1]), command[2]);
				if(iRet == ERR_INVALID_PARAMETER)
				{
					printf("ERROR: Invalid parameters. Please refer man page\n");
				}
				if(iRet == ERR_FILE_NOT_EXISTS)
				{
					printf("ERROR: File does not exist\n");
				}
				if(iRet == ERR_PERMISSION_DENIED)
				{
					printf("ERROR: File does not have write permission\n");
				}
				if(iRet == ERR_INSUFFICIENT_SPACE)
				{
					printf("ERROR: Not enough space left in file\n");
				}
				if(iRet > 0)
				{
					printf("Number of bytes written: %d\n", iRet);
				}
			}
			/* CVFS > read fd size */
			else if(strcmp("read", command[0]) == 0)
			{
				iRet = read_file(atoi(command[1]), atoi(command[2]));
				if(iRet == ERR_INVALID_PARAMETER)
				{
					printf("ERROR: Invalid parameters. Please refer man page\n");
				}
				if(iRet == ERR_FILE_NOT_EXISTS)
				{
					printf("ERROR: File does not exist\n");
				}
				if(iRet == ERR_PERMISSION_DENIED)
				{
					printf("ERROR: File does not have read permission\n");
				}
				if(iRet == ERR_INSUFFICIENT_DATA)
				{
					printf("ERROR: No more data left to read\n");
				}
				if(iRet > 0)
				{
					printf("Number of bytes read: %d\n", iRet);
				}
			}
			else
			{
				printf("Command not found\n");
				printf("Please refer help option to get more information\n");
			}
		} /* End of else if 3*/
		else
		{
			printf("Command not found\n");
			printf("Please refer help option to get more information\n");
		} /* End of else*/
	} /* End of while*/

	return 0;
}
