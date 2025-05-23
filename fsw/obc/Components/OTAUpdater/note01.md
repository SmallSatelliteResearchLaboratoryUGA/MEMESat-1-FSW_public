### Apparent Control Flow for the OTAU Component Design


### Stage #0 -> Stage #1

- Updater_dir does not exist 
    - attempt to create updater_dir ? (log & proceed) : (log & return)
        - create status object to take return from dir creation attempt
        - evaluate status object
            - if created, proceed
            - if fault, critical error 
    - attempt to create updater_dir/tracking.txt ? (log & proceed) : (log & return )
        - create status object to take return from file creation attempt
        - evaluate status object 
            - if created, proceed 
            - if fault, critical error 
        - populate tracking txt with necessary base parameters. 

### Stage #1 -> Stage #2 

- on audit 
    - read directory size and store into variable
    - create list object of length ^ for reading in dir contents of 'updater_dir'
    - read in directory contents to populate list 
    - create vector with <path, <param, value>> for storing contents of index.txt files held in each subdir 
    - iterate through list of directory contents 
        - if entry is a file, log. Here, there should be only one file: tracking.txt. (make custom file type); 
        - if entry is tracking.txt, 
            - take file size 
            - create buffer of size retained 
            - read file into buffer 
            - ingest buffer contents and read parameters into a key:value vector. 
        - if entry is a subdirectory 
            - if subdirectory vector does not exist, create one with size as length of directory contents minus 1. 
            - read subdirectory size and store into variable 
            - create list object of length ^ for reading in subdir contents of 'updater_dir/{subdir}'. 
            - iterate through list contents
                - check if there is an entry with the name 'tracking.txt'
                - check if there is an entry with the name '{parent_dir}.concat'
                - check if there is an entry with the name '{*}.seg'
                    - count how many files meet this specification and save to var
            - if tracking.txt is not present, cleanup dir and remove path.
            - if tracking.txt is present,
                - read file size into variable
                - create buffer of file size
                - ingest buffer contents and read parameters into a <param, value> vector
                - once buffer contents are read and parsed, add the information to the vector of type <path, <param, value>>
                    - keep parameters read local to the process
            - if '{*}.seg' file(s) is/are present
                - iterate through list contents again and populate '{*}.seg' paths into vector created
            - if '{parent_dir}.concat' file is present,
                - read file size into variable
                - if file size is lesser than the target file size param,
                    - proceed with segment file evaluation
                - if the file size is equal to the target file size param,
                    - proceed with checksum comparison
                - if the file size is greater than the target file size param,
                    - log and remove subdirectory from filesystem
            - else if '{*}.seg' file(s) is/are present,
                - create '{parent_dir}.concat' file

            - [ segment file evaluation : (<path, <param, value>>, <segment_file_paths>, '{parent_dir}.concat', target_file_size) ]
                - open '{path}/{path}.concat' for appending
                - order segment_file_paths lexicographically and store in new list.
                - iterate through segment_file_paths_ordered
                    - read in file size of segment file
                        - if file is lesser than or equal to the target_segment_size parameter,
                            - read in the segment file size and store in variable
                            - create buffer to read the file contents into
                            - read file contents into buffer
                            - write buffer to the '{path}/{path}.concat' file
                            - remove .seg file from filesystem
                        - else if file is greater than the target_segment_size parameter,
                            - remove file from filesystem and
                            - trigger flag to remove any following files in the ordered list.
                            - log each time.
                        - keep a running counter of the target_file_size provided and the size of contents being added to it
                            - if target is greater or equal to the counter, nominal
                            - else, log and stop concatenation.
                                - remove subdir and contents from filesystem
                - read in '{parent_dir}.concat' file size
                    - if '{parent_dir}.concat' file size is lesser than target file size, proceed.
                        - update parameter held in the tracking.txt vector to reflect this.
                    - if '{parent_dir}.concat' file size is equal to the target_file_size, log and
                        - proceed with checksum comparison
                    - if '{parent_dir}.concat' file size is greater than the target_file_size, log and
                        - remove parent_dir from filesystem contents.

            - [ checksum comparison : (<path, <param, value>>, '{parent_dir}.concat', md5_checksum) ]
                - grab file size of file at '{parent_dir}.concat'
                - create buffer of size ^
                - read contents of file into buffer
                - compute md5 checksum of the provided buffer file contents
                - compare computed md5 checksum with param:value previously held within the index.txt file of the corresponding path.
                - if computed md5 checksum matches
                    - change '{parent_dir}.concat' to '{parent_dir}.{provided file descriptor}'
                    - move file to 'updater_dir/complete'.
                    - remove subdirectory filepath from filesystem
                    - update parameters held in the tracking.txt vector to reflect this.
                - if computed md5 checksum does not match,
                    - log and remove subdirectory path from filesystem
                    - update parameters held in the tracking.txt vector to reflect this.