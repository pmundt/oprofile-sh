/* $Id$ */
/* COPYRIGHT (C) 2000 THE VICTORIA UNIVERSITY OF MANCHESTER and John Levon
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <stdlib.h> 
 
#include "opd_util.h"
#include "../config.h"

/* FIXME: move into util/ ?? */
 
/**
 * opd_mangle_filename - mangle a file filename
 * @image_name: a path name to the image file
 * @app_name: a path name for the owner image
 * of this image or %NULL if no owner exist
 *
 * Returns a char* pointer to the mangled string. Caller
 * is respomsible for freeing this string.
 *
 */
char* opd_mangle_filename(const char * image_name, const char * app_name)
{
	char *mangled;
	char *c;
	size_t len;

	len = strlen(OP_SAMPLES_DIR) + 2 + strlen(image_name) + 32;
	if (app_name)
		len += strlen(app_name) + 2;

	mangled = xmalloc(len);
	
	strcpy(mangled, OP_SAMPLES_DIR);
	strcat(mangled, "/");

	c = mangled + strlen(mangled);

	if (app_name) {
		strcat(mangled, app_name);
		/* a double OPD_MANGLE_CHAR used as marker ? */
		strcat(mangled, "//");
	}

	strcat(mangled, image_name);

	for ( ; *c != '\0'; ++c) {
		if (*c == '/')
			*c = OPD_MANGLE_CHAR;
	}

	return mangled;
}

/**
 * opd_do_open_file - open a file
 * @name: file name
 * @mode: mode string
 * @fatal: is failure fatal or not
 *
 * Open a file name.
 * Returns file handle or %NULL on failure.
 */
FILE *opd_do_open_file(const char *name, const char *mode, int fatal)
{
	FILE *fp;

	fp = fopen(name, mode);

	if (!fp) {
		if (fatal) { 
			fprintf(stderr,"oprofiled:opd_do_open_file: %s: %s", name, strerror(errno)); 
			exit(EXIT_FAILURE);
		} 
	}

	return fp;	
} 

/**
 * opd_close_file - close a file
 * @fp: file pointer
 *
 * Closes a file pointer. A non-fatal
 * error message is produced if the
 * close fails.
 */ 
void opd_close_file(FILE *fp)
{
	if (fclose(fp)) {
		perror("oprofiled:opd_close_file: ");
	}
}

/**
 * opd_do_read_file - read a file
 * @fp: file pointer
 * @buf: buffer
 * @size: size in bytes to read
 * @fatal: is failure fatal or not
 *
 * Read from a file. It is considered an error 
 * if anything less than @size bytes is read.
 */
void opd_do_read_file(FILE *fp, void *buf, size_t size, int fatal)
{
	size_t count;

	count = fread(buf, size, 1, fp);

	if (fatal && count != 1) {
		if (feof(fp))
			fprintf(stderr,"oprofiled:opd_read_file: read less than expected %d bytes\n", size);
		else
			fprintf(stderr,"oprofiled:opd_read_file: error reading\n");
		exit(EXIT_FAILURE);
	}
}
 
/**
 * opd_do_read_u8 - read a byte from a file
 * @fp: file pointer
 *
 * Read an unsigned byte from a file.
 * 0 is returned if the read fails in any way.
 */ 
u8 opd_read_u8(FILE *fp)
{
	u8 val = 0;
	opd_do_read_file(fp, &val, sizeof(u8), 0);

	return val; 
}
 
/**
 * opd_do_read_u16_he - read two bytes from a file
 * @fp: file pointer
 *
 * Read an unsigned two-byte value from a file.
 * 0 is returned if the read fails in any way.
 *
 * No byte-swapping is done. 
 */ 
u16 opd_read_u16_he(FILE *fp)
{
	u16 val = 0;
	opd_do_read_file(fp, &val, sizeof(u16), 0);
	return val;
}
 
/**
 * opd_do_read_u32_he - read four bytes from a file
 * @fp: file pointer
 *
 * Read an unsigned four-byte value from a file.
 * 0 is returned if the read fails in any way.
 *
 * No byte-swapping is done. 
 */ 
u32 opd_read_u32_he(FILE *fp)
{
	u32 val = 0;
	opd_do_read_file(fp, &val, sizeof(u32), 0);
 
	return val;
}
 
/**
 * opd_write_file - write to a file
 * @fp: file pointer
 * @buf: buffer
 * @size: nr. of bytes to write
 *
 * Write @size bytes of buffer @buf to a file.
 * Failure is fatal.
 */ 
void opd_write_file(FILE *fp, const void *buf, size_t size)
{
	size_t written;

	written = fwrite(buf, size, 1, fp);

	if (written != 1) {
		fprintf(stderr,"oprofiled:opd_write_file: wrote less than expected: %d bytes.\n", size);
		exit(EXIT_FAILURE);
	}
}
 
/**
 * opd_write_u8 - write a byte to a file
 * @fp: file pointer
 * @val: value to write
 *
 * Write the unsigned byte value @val to a file.
 * Failure is fatal.
 */ 
void opd_write_u8(FILE *fp, u8 val)
{
	opd_write_file(fp, &val, sizeof(val));
}
 
/**
 * opd_write_u16_he - write two bytes to a file
 * @fp: file pointer
 * @val: value to write
 *
 * Write an unsigned two-byte value @val to a file.
 * Failure is fatal.
 *
 * No byte-swapping is done. 
 */
void opd_write_u16_he(FILE *fp, u16 val)
{
	opd_write_file(fp, &val, sizeof(val));
}
 
/**
 * opd_write_u23_he - write four bytes to a file
 * @fp: file pointer
 * @val: value to write
 *
 * Write an unsigned four-byte value @val to a file.
 * Failure is fatal.
 *
 * No byte-swapping is done. 
 */ 
void opd_write_u32_he(FILE *fp, u32 val)
{
	opd_write_file(fp, &val, sizeof(val));
}
 
/**
 * opd_read_int_from_file - parse an ASCII value from a file into an integer
 * @filename: name of file to parse integer value from
 *
 * Reads an ASCII integer from the given file. All errors are fatal.
 * The value read in is returned.
 */
u32 opd_read_int_from_file(const char *filename) 
{
	FILE *fp;
	u32 value;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "opd_read_int_from_file: Failed to open %s, reason %s\n", filename, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fscanf(fp, "%u", &value) != 1) {
		fclose(fp);
		fprintf(stderr, "opd_read_int_from_file: Failed to convert contents of file %s to integer\n", filename);
		exit(EXIT_FAILURE);
	}

	fclose(fp);

	return value;
}

/**
 * opd_get_fsize - get size of file
 * @file: file name
 * @fatal: exit on error
 *
 * Returns the size of the named file in bytes.
 * Failure is fatal if @fatal is %TRUE.
 */ 
off_t opd_get_fsize(const char *file, int fatal)
{
	struct stat st;

	if (stat(file, &st)) {
		if (!fatal) 
			return 0;
		 
		fprintf(stderr,"opd_get_fsize: stat failed\n");
		exit(EXIT_FAILURE);
	}

	/* PHE FIXME caller can not make any difference between failure and
	 * zero file size when fatal != 0 */
	return st.st_size;
}

/**
 * opd_get_mtime - get mtime of file
 * @file: file name
 *
 * Returns the mtime of the given file or 0 on failure
 */
time_t opd_get_mtime(const char * file)
{
	struct stat st;

	if (stat(file, &st))
		return 0;

	return st.st_mtime;
}


/**
 * opd_get_time - get current date and time
 *
 * Returns a string representing the current date
 * and time, or %NULL on error.
 *
 * The string is statically allocated and should not be freed.
 */ 
char *opd_get_time(void)
{
	time_t t = time(NULL);

	if (t == -1)
		return NULL;

	return ctime(&t);
}
 
/**
 * opd_get_line - read an ASCII line from a file
 * @fp: file pointer 
 *
 * Get a line of ASCII text from a file. The file is read
 * up to the first \0 or \n. A trailing \n is deleted.
 * 
 * Empty lines are not handled.
 *
 * Returns the dynamically-allocated string containing
 * that line. At the end of a file a string "" will
 * be returned.
 *
 * In *both* cases, the string must be free()d at some
 * point.
 */ 
char *opd_get_line(FILE *fp)
{
	char *buf;
	char *cp;
	int c;
	/* average allocation is about 31, so 64 should be good */
	size_t max = 64;

	buf = xmalloc(max);
	cp = buf; 

	while (1) {
		switch (c = fgetc(fp)) { 
			case EOF:
			case '\n':
			case '\0':
				*cp = '\0';
				return buf;
				break;

			default:
				*cp = (char)c;
				cp++;
				if (((size_t)(cp - buf)) == max) {
					buf = xrealloc(buf, max + 64);
					cp = buf+max;
					max += 64;
				}
				break;
		}
	}
}

/**
 * opd_open_device - open a special char device for reading
 * @name: file name of device file
 * @fatal: fatal or not
 *
 * Open the special file @name. Returns the file descriptor
 * for the file or -1 on error.
 */ 
fd_t opd_open_device(const char *name, int fatal)
{
	fd_t fd;
 
	fd = open(name, O_RDONLY);
	if (fatal && fd == -1) {
		fprintf(stderr,"oprofiled:opd_open_device: %s: %s\n", name, strerror(errno)); 
		exit(EXIT_FAILURE);
	}

	return fd;
}

/**
 * opd_close_device - close a special char device
 * @devfd: file descriptor of device
 *
 * Close a special file. Failure is fatal.
 */ 
void opd_close_device(fd_t devfd)
{
	if (close(devfd)) {
		perror("oprofiled:opd_close_device: ");
		exit(EXIT_FAILURE);
	}	
} 
 
/**
 * opd_read_device - read from a special char device
 * @devfd: file descriptor of device
 * @buf: buffer
 * @size: size of buffer
 * @seek: seek to the start or not 
 *
 * Read @size bytes from a device into buffer @buf.
 * A seek to the start of the device file is done first
 * if @seek is non-zero, then a read is requested in one 
 * go of @size bytes.
 *
 * It is the caller's responsibility to do further opd_read_device()
 * calls if the number of bytes read is not what is requested
 * (where this is applicable).
 *
 * The number of bytes read is returned, or a negative number
 * on failure (in which case errno will be set). If the call is
 * interrupted, then errno will be EINTR, and the client should
 * arrange for re-starting the read if necessary.
 */ 
ssize_t opd_read_device(fd_t devfd, void *buf, size_t size, int seek)
{
	ssize_t count;
	
	if (seek) 
		lseek(devfd,0,SEEK_SET);
 
	count = read(devfd, buf, size);

	if (count < 0 && errno != EINTR && errno != EAGAIN) {
		perror("oprofiled:opd_read_device: ");
		exit(EXIT_FAILURE);
	}
 
	return count;
}

/**
 * opd_move_regular_file - move file between directory
 * @new_dir: the destination directory
 * @old_dir: the source directory
 * @name: the filename
 *
 * move the file @old_dir/@name to @new_dir/@name iff
 * @old_dir/@name is a regular file
 *
 * if renaming succeed zero or the file is not 
 * a regular file is returned
 */ 
int opd_move_regular_file(const char *new_dir, 
			  const char *old_dir, const char *name)
{
	int ret = 0;
	struct stat stat_buf;

	char * src = xmalloc(strlen(old_dir) + strlen(name) + 2);
	char * dest = xmalloc(strlen(new_dir) + strlen(name) + 2);

	strcpy(src, old_dir);
	strcat(src, "/");
	strcat(src, name);

	strcpy(dest, new_dir);
	strcat(dest, "/");
	strcat(dest, name);

	if (!stat(src, &stat_buf) && S_ISREG(stat_buf.st_mode))
		ret = rename(src, dest);

	free(src);
	free(dest);

	return ret;
}
 

/**
 * opd_read_lock_file - read a lock file
 *
 * Return the pid written in the given lock file,
 * or 0 if it doesn't exist.
 */
pid_t opd_read_lock_file(const char * file)
{
	FILE * fp;
	pid_t value;
 
	fp = fopen(file, "r");
	if (fp == NULL)
		return 0;
 
	if (fscanf(fp, "%u", &value) != 1) {
	        fclose(fp);
		return 0;
        }
 
	fclose(fp);
 
        return value;
}
 
 
/**  
 * opd_write_lock_file - write a lock file
 * \return errno on failure, or 0 on success
 * 
 * Write the pid into the given lock file.
 */ 
int opd_write_lock_file(const char * file)
{
	FILE * fp;

	if (opd_get_fsize(file, 0) != 0)
		return EEXIST;
 
	fp = fopen(file, "w");
	if (!fp)
		return errno;

	fprintf(fp, "%d", getpid());
	fclose(fp);

	return 0;
}
