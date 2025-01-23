#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"


/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Stego Image file
 * Output: FILE pointer for above file
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_open_files(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files \n");
    /*opening the decoded image file*/
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"r");

    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n",decInfo->stego_image_fname);
    }

}

/*Function to skip bmp header*/
Status skip_bmp_header(FILE *fptr_image)
{
    fseek(fptr_image,54,SEEK_SET);
    if(ftell(fptr_image)!=54)
    {
        return e_failure;
    }
    else
    {
        return e_success;//return success
    }
}

/*Decoding the magic string*/
Status decode_magic_string(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature \n");
    char decode_str[10];
    int len=strlen(MAGIC_STRING);
    int i;
    for( i=0;i<len;i++)
    {
        char image_buffer[8];
        /*read 8 bytes from stego_image*/
        fread(image_buffer,1,8,decInfo->fptr_stego_image);
        /*storing decoded message*/
        decode_str[i]=decode_lsb_to_byte(image_buffer);
    }
    decode_str[i]='\0';
    char ch[10];
    //printf("Enter the magic string: ");
    scanf("%s",ch);/*asking the user to enter the magic string*/

    /*comparing the user entered and original magic string*/
    if (strcmp(decode_str, ch) == 0)
    {
        printf("INFO: Done \n");
        return e_success;  // Magic strings match
    }
    else
    {
        printf("INFO: Magic String Not Matched\n");
        return e_failure;  // Magic strings do not match
    }
}

/*Decoding the file extension size*/
Status decode_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    /*read 32 bytes from stego_image*/
    fread(image_buffer,1,32,decInfo->fptr_stego_image);
    /*storing decoded message*/
    decInfo->extn_size=decode_lsb_to_int(image_buffer);
    return e_success;
}

/*Decoding secret file extension*/
Status decode_secret_file_extn( DecodeInfo *decInfo)
{
    printf("INFO: Decoding Output File Extension \n");
    int i;
    for(i=0;i<decInfo->extn_size;i++)
    {
    char image_buffer[8];
    /*read 8 bytes from stego_image*/
    fread(image_buffer,1,8,decInfo->fptr_stego_image);
    /*storing decoded message*/
    decInfo->extn_secret_file[i]=decode_lsb_to_byte(image_buffer);
    }
    decInfo->extn_secret_file[i]='\0';
    printf("INFO: Done \n");
    return e_success;
   // printf("%s",decInfo->extn_secret_file);
}

/*Opening the output file*/
Status open_output_file(DecodeInfo *decInfo)
{
    strcpy(decInfo->secret_data,decInfo->file_name);
    /*merging the extention with output*/
    strcat(decInfo->secret_data,decInfo->extn_secret_file);
    /*opening the file in write mode*/
    decInfo->fptr_secret=fopen(decInfo->secret_data,"w");
    if(decInfo->file_present == 1)
    {
        printf("INFO: Output File not mentioned. Creating %s as default\n",decInfo->secret_data);
        printf("INFO: Opened %s\n",decInfo->secret_data);
    }
    else
    {
        printf("INFO: Opened %s\n",decInfo->secret_data);
    }
    printf("INFO: Done. Opened all required files\n");
    return e_success;
}

/*Decoding the secret file size*/
Status decode_secret_file_size( DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Size \n",decInfo->secret_data);
    char image_buffer[32];
    /*read 32 bytes from stego_image*/
    fread(image_buffer,1,32,decInfo->fptr_stego_image);
    /*storing decoded message*/
    decInfo->file_size=decode_lsb_to_int(image_buffer);
    printf("INFO: Done \n");
    return e_success;
}

/*Decoding the secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Data \n",decInfo->secret_data);
    int i;
    for(i=0;i<decInfo->file_size;i++)
    {
    char image_buffer[8];
    /*read 8 bytes from stego_image*/
    fread(image_buffer,1,8,decInfo->fptr_stego_image);
    /*storing decoded message*/
    char ch=decode_lsb_to_byte(image_buffer);
    fputc(ch,decInfo->fptr_secret);
    }
    // decInfo->secret_data[i]='\0';
    // printf("%s",decInfo->secret_data);
    printf("INFO: Done \n");
    return e_success;
}

int decode_lsb_to_byte( char *image_buffer)
{
    char decoded=0;
    for(int i=0;i<8;i++)
    {
        char lsb = (image_buffer[i]&1);
        decoded |=(lsb << (7-i));
    }
    
    return decoded;
}

int decode_lsb_to_int( char *image_buffer)
{
    int decoded=0;
    for(int i=0;i<32;i++)
    {
        char lsb = (image_buffer[i]&1);
        decoded |=(lsb << (31-i));
    }
    
    return decoded;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(decode_open_files(decInfo)==e_failure)
    {
        return e_failure;
    }

    if(skip_bmp_header(decInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }

    if(decode_magic_string(decInfo)==e_failure)
    {
        return e_failure;
    }

    if(decode_file_extn_size(decInfo)==e_failure)
    {
        return e_failure;
    }

    if(decode_secret_file_extn(decInfo)==e_failure)
    {
        return e_failure;
    }

    if(open_output_file(decInfo)==e_failure)
    {
        return e_failure;
    }

    if(decode_secret_file_size(decInfo)==e_failure)
    {
        return e_failure;
    }

    if(decode_secret_file_data(decInfo)==e_failure)
    {
        return e_failure;
    }

}
