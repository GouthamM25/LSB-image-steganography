#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    printf("INFO: Opening required files \n");
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n",encInfo->src_image_fname);
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n",encInfo->secret_fname);
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n",encInfo->stego_image_fname);
    }
    printf("INFO: Done \n");
    // No failure return e_success
    return e_success;
}

/*checking the capacity of image file */
Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: ## Encoding Procedure Started ## \n");
    // Find image size
    int img_size = get_image_size_for_bmp(encInfo->fptr_src_image); 
    // Find secret file size
    printf("INFO: Checking for %s size \n",encInfo->secret_fname);
    fseek(encInfo->fptr_secret, 0, SEEK_END);  
    long secretsize = ftell(encInfo->fptr_secret);
    if(secretsize==0)
    {
        return e_failure;
    }  
    else
    {
        printf("INFO: Done. Not Empty \n");
    }
    fseek(encInfo->fptr_secret, 0, SEEK_SET);  

    // Calculate the magic string length
    int magic_len = strlen(MAGIC_STRING); 

    // Calculate the extension length
    int extn_len = strlen(encInfo->extn_secret_file);  

    // Total data size to encode (in bits)
    printf("INFO: Checking for %s capacity to handle %s \n",encInfo->src_image_fname,encInfo->secret_fname);
    int total_data_size = 54  + ((magic_len * 8) + (sizeof(extn_len)*8) + (extn_len*8)  + (sizeof(secretsize)*8) + (secretsize*8) );
    //printf("%d\n",total_data_size);

    // Compare total data size with image capacity
    if (img_size >= total_data_size)
    {
        printf("INFO: Done. Found OK \n");
        return e_success;  // Image has enough capacity to hold the data
    }
    else
    {
        return e_failure;  // Image does not have enough capacity to hold the data
    }
}


/*copying the first 54 bytes (header) of beautiful.bmp file*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying image header \n");
    /*rewind both src and dest file pointers*/
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    char buffer[54];
    /*read 54 bytes data from src file*/
    int read=fread(buffer,1,54,fptr_src_image);
    /*write the 54 bytes to dest file*/
    int ret=fwrite(buffer,1,54,fptr_dest_image);
    if (ret!=54)
    {
       return e_failure;/*not copying 54 bytes*/
    }
    else
    {
        printf("INFO: Done \n");
        return e_success;/*copied 54 bytes*/
    }
}

/*Start the encoding procedure */
/*Encode the magic string*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature \n");
    int len=strlen(MAGIC_STRING);/*find length of the magic string*/
    for(int i=0;i<len;i++)
    {
        char image_buffer[8];
        /*read 8 bytes data from src file*/
        fread(image_buffer,1,8,encInfo->fptr_src_image);
        /*call the encode_byte_to_lsb function*/
        encode_byte_to_lsb(magic_string[i],image_buffer);
        /*write 8 bytes to stego_image*/
        fwrite(image_buffer,1,8,encInfo->fptr_stego_image);
    }
    printf("INFO: Done \n");
    return e_success;
    
}

/*Encodiing secret file extension length*/
Status encode_secret_file_extn_length(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Extension size \n",encInfo->secret_fname);
    const char *extn = strchr(file_extn, '.'); 
    int extn_length=strlen(extn);
    char image_buffer[32];
    /*read 32 bytes from src image*/
    fread(image_buffer,1,32,encInfo->fptr_src_image);
    /*call encode_int_to_lsb function*/
    encode_int_to_lsb(extn_length,image_buffer);
    /*write 32 bytes to stego_image*/
    fwrite(image_buffer,1,32,encInfo->fptr_stego_image);
    printf("INFO: Done \n");
    return e_success;    
}


/*Encoding secret file extension*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Extension \n",encInfo->secret_fname);
    const char *extn = strchr(file_extn, '.'); 
    int extn_length=strlen(extn);
    for(int i=0;i<extn_length;i++)
    {
        char image_buffer[8];
        /*read 8 bytes data from src file*/
        fread(image_buffer,1,8,encInfo->fptr_src_image);
        /*call the encode_byte_to_lsb function*/
        encode_byte_to_lsb(extn[i],image_buffer);
        /*write 8 bytes data from src file*/
        fwrite(image_buffer,1,8,encInfo->fptr_stego_image);
    }
    printf("INFO: Done \n");
    return e_success;
}

/*function to find size of secret file*/
uint get_file_size(FILE *fptr)
{
    FILE *file = fopen("secret.txt", "r");  
    if (!file) 
    {
        perror("Unable to open file");
        return 1;  
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);  
    fclose(file); 
    return size;
   
}

/*Encoding secret file size*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Size \n",encInfo->secret_fname);
    int size=get_file_size(encInfo->fptr_secret);
    char image_buffer[32];  
    /*read 32 bytes data from src file*/
    fread(image_buffer, 1, 32, encInfo->fptr_src_image); 
    encode_int_to_lsb(size, image_buffer);
    /*read 32 bytes data to stego  file*/
    fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image) ;
    printf("INFO: Done \n");
    return e_success;
}

/*Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Data \n",encInfo->secret_fname);
    // Get the size of the secret file
    long secret_file_size = get_file_size(encInfo->fptr_secret);

    // Buffer to hold 1 byte of secret data
    char secret_byte;

    // Buffer to hold 8 bytes of image data (for encoding 1 byte of secret data)
    char image_buffer[8];

    // Loop through the secret file, reading and encoding byte by byte
    for (long i = 0; i < secret_file_size; i++)
    {
        // Read 1 byte from the secret file
        fread(&secret_byte, 1, 1, encInfo->fptr_secret);

        // Read 8 bytes from the source image
        fread(image_buffer, 1, 8, encInfo->fptr_src_image);

        // Encode the current secret byte into the 8 bytes of image data
        encode_byte_to_lsb(secret_byte, image_buffer);

        // Write the modified image buffer to the stego image
        fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image);
    }

    // Return success if the whole file has been encoded
    printf("INFO: Done \n");
    return e_success;

}

/*Copying remaining data*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data \n");
    int ch;
    while((ch=fgetc(fptr_src))!=EOF)
    {
        fputc(ch,fptr_dest);
    }
    printf("INFO: Done \n");
    return e_success;
    /*
    char buffer[8];
    int ret=1;
    while(ret!=0)
    {
    ret=fread(buffer,1,1,fptr_src);
    fwrite(buffer,1,1,fptr_dest);
    }
    return e_success;
*/

}


Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
       /* clear the lsb of image buffer*/
        image_buffer[i]=(image_buffer[i]&0xFE);
       /*get 1 bit from MSB of data*/
        char ch = (uint)(data & (1<<7-i))>>(7-i);
        /*merge the 1 bit MSB data and image_buffer data*/
        image_buffer[i] = image_buffer[i] | ch;
    }
}

Status encode_int_to_lsb(int data,char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
        image_buffer[i]=(image_buffer[i]&0xFE);
        char ch = (uint)(data & (1<<31-i))>>(31-i);
        image_buffer[i] = image_buffer[i] | ch;
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    
    if(open_files(encInfo) == e_failure)
    {
        return e_failure;
    }
    
    if(check_capacity(encInfo) == e_failure)
    {
        return e_failure;
    }
    
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }
    
    if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure)
    {
        return e_failure;
    }
    
    const char *file_extn = strchr(encInfo->secret_fname, '.');
    if (file_extn == NULL)
    {
        printf("Error: No file extension found in the secret file name!\n");
        return e_failure;
    }

    if(encode_secret_file_extn_length(file_extn,encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_extn(file_extn,encInfo) == e_failure)
    {
        return e_failure;
    }

    long secret_file_size = get_file_size(encInfo->fptr_secret);
    if (encode_secret_file_size(secret_file_size,encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
    printf("Error: Failed to copy remaining image data!\n");
    return e_failure;
    }
}


