#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file from stego Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    int extn_size;
    int file_size;
    char *file_name;
    int file_present;
    

    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

} DecodeInfo;


/* Decoding function prototypes */

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p files */
Status decode_open_files(DecodeInfo *decInfo);

/* Skip BMP header */
Status skip_bmp_header(FILE *fptr_image);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn( DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size( DecodeInfo *decInfo);

/*opening file to store decoded data*/
Status open_output_file(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode LSB to retrieve a byte */
int decode_lsb_to_byte(char *image_buffer);

/* Decode LSB to retrieve an integer */
int decode_lsb_to_int( char *image_buffer);

#endif
