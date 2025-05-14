#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int reverse(int num) {
    char *ptr = (char *)&num;
    for (int i = 0; i < 2; i++) {
        char temp = ptr[i];
        ptr[i] = ptr[4 - 1 - i];
        ptr[4 - 1 - i] = temp;
    }
    return num;
}

void show_help() {
    printf("--------------------------------------------------\n");
    printf("MP3 Tag Reader and Editor\n");
    printf("Usage:\n");
    printf("  ./mp3tool -v <file.mp3>              View ID3 tags\n");
    printf("  ./mp3tool -e <file.mp3> <TAG> <VAL>  Edit/Add ID3 tag\n");
    printf("  ./mp3tool -h                         Show help\n");
    printf("--------------------------------------------------\n");
}
const char* short_to_tag(const char* short_opt) {
    if (strcmp(short_opt, "-t") == 0) return "TIT2"; // Title
    if (strcmp(short_opt, "-a") == 0) return "TPE1"; // Artist
    if (strcmp(short_opt, "-al") == 0) return "TALB"; // Album
    if (strcmp(short_opt, "-g") == 0) return "TCON"; // Genre
    if (strcmp(short_opt, "-y") == 0) return "TYER"; // Year
    if (strcmp(short_opt, "-c") == 0) return "COMM"; //composer
    return NULL;
}

void view_tags(const char *file_name) {
    FILE *fp = fopen(file_name, "rb"); 
    if (!fp) {
        printf("Error: Could not open file\n");
        return;
    }
   //to check ID3 or not
    char header[3];
    fread(header, 1, 3, fp);
    if (strncmp(header, "ID3", 3) != 0) {
        printf("The file does not have an ID3 tag\n");
        fclose(fp);
        return;
    }

    printf("ID3 tag found in file.\n");
    fseek(fp, 10, SEEK_SET);  //skip header 10 bytes

    for (int i = 0; i < 6; i++) {
        char tag[5] = {0};
        if (fread(tag, 1, 4, fp) != 4)
            break;

        int size;
        if (fread(&size, 1, 4, fp) != 4)
            break;

        size = reverse(size); //convert to little endian

        fseek(fp, 3, SEEK_CUR); // skip flags,2 for flags,1 for null
        if (size <= 0)
            break;

        char *content = malloc(size); //dynamic memory allocation
        fread(content, 1, size - 1, fp);
        content[size - 1] = '\0';

        if (strncmp(tag, "TIT2", 4) == 0) //compare tag contain TIT2 or not
            printf("Title: %s\n", content);
        else if (strncmp(tag, "TPE1", 4) == 0) //compare tag contain TPE1 or not
            printf("Artist: %s\n", content);
        else if (strncmp(tag, "TALB", 4) == 0)  //compare tag contain TALB or not
            printf("Album: %s\n", content);
        else if (strncmp(tag, "TCON", 4) == 0) //compare tag contain TCON or not
            printf("Genre: %s\n", content);
        else if (strncmp(tag, "TYER", 4) == 0) //compare tag contain TYER or not
            printf("Year: %s\n", content);
        else if (strncmp(tag, "COMM", 4) == 0) //compare tag contain TCOM or not
            printf("Composer:%s\n", content);
        else
            printf("Other (%s): %s\n", tag, content);

        free(content);
    }

    fclose(fp);
}
void edit_tag(const char *file_name, const char *tag, const char *content)
 {
    FILE *in = fopen(file_name, "rb");
    if (!in) {
        printf("Error: Could not open file\n");
        return;
    }

    FILE *out = fopen("sample1.mp3", "wb"); 
    if (!out) {
        printf("Error: Could not create temp file\n");
        fclose(in);
        return;
    }

    char buffer[10]; //header
    fread(buffer, 1, 10, in); //reader 10 bytes from header
    fwrite(buffer, 1, 10, out); //write 10 bytes into new file
    char flags[3];
    int tag_found=0; //flag
    while (1)
    {
        char frame[5] = {0};
        if (fread(frame, 1, 4, in) != 4)// read tags
            break;
        frame[4] = '\0';     
        //read size from input file
        int size;
        if (fread(&size, 1, 4, in) != 4)
            break;
        size = reverse(size); //convert into little endian
    

        if (size <= 0)
            break;

        char *data = malloc(size); //dynamic memory allocation
        if(!data)

        {
            printf("memory allocation failed\n");
            break;
        }
        if (fread(flags, 1, 3, in) != 3) break;//read flags
        fread(data, 1, size-1, in); //read data

        if (size <= 0 || feof(in))
            break;

        if (strncmp(frame,tag, 4) == 0)
        {

            printf("modifyin frame:%s\n", frame);
            fwrite(frame, 1, 4, out);

            // Write the new size of the frame content
            int new_size = strlen(content) + 1; // Length of the new data includong null
            int new_size_be = reverse(new_size);         // conversion
               // reverse(new_size);
            fwrite(&new_size_be, 1, 4, out);
            
            fwrite(flags, 1, 3, out); // flag write
            fwrite(content, 1, strlen(content), out);
            tag_found=1;
            break;
            }
        else
        {
            // Write original frame as is
            fwrite(frame, 1, 4, out);
            int size_be = reverse(size);
            fwrite(&size_be, 1, 4, out);
            fwrite(flags, 1, 3, out);
            fwrite(data, 1, size-1, out);
        }
        free(data);
        
    }
    char ch;
        while(fread(&ch,1,1,in)>0)
        {
            fwrite(&ch,1,1,out);
        }
    fclose(in);
    fclose(out);
    printf("Output written to sample1.mp3\n");
    if (!tag_found) 
    {
        printf("Warning: Tag %s not found in the file.\n",tag);
    }
    //....... delet file and rename newfile......
    const char *old_file="file_name";
    const char *new_file="sample1.mp3";
     //first remove the original file
     if(remove(old_file)==0)
     {
        printf("error while saving data in delete");
        return;
     }
     //then rename the new file to the original files name
     if(rename("sample1.mp3","sample.mp3"))
     {
        printf("error while saving data in rename");
        return;
     }
 }
 int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Missing arguments\n");
        show_help();
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0) {
        show_help();
    }
     else if (strcmp(argv[1], "-v") == 0 && argc >= 3)
    {
        view_tags(argv[2]);
    } else if (strcmp(argv[1], "-e") == 0 && argc >= 5) 
    {
        const char* real_tag = short_to_tag(argv[3]);
        if (real_tag == NULL)
        {
            printf("Error: Unknown tag short name. Use -t, -a, -al, -g, -y\n");
            return 1;
        }
       edit_tag(argv[2],real_tag, argv[4]);
    } else {
        printf("Invalid command or missing arguments.\n");
        show_help();
        return 1;
    }

    return 0;
}
    