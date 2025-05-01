# OTAU

## Requirements

>> 1) OTAU needs to bind to scheduler to run at a sub hz rate.
>> 2) OTAU needs to connect to file recycler to cleanup compiled dirs.
>> 3) OTAU needs to report the following telemetry </br>
> a. n_objects building </br>
> b. n_objects built </br>
> c. n_objects tracked </br>
> d. n_times a file difference has been noticed </br> 
>> 4) OTAU needs to do the following </br>
> a. track the number of objects in the respective working directory </br>
> b. report all objects being tracked in a file at the base of the working directory </newline>
> titled 'tracking.txt'</br>
> c. ingest the index.txt report parameters of the target file currently being built </newline>
> (held in the respective object directory) </br>
> d. save any current audit statistics in a file of the respective object directory labeled 'summary.txt' </newline>
> e. identify when a file has been fully received in its segments and compare the culmination against a </newline>
> md5 checksum. </br>
> f. if the object group satisfies the checksum comparison, then the file is converted to a target file format </newline>
> g. the target file may either sit in the segment directory until further action from gds controls or have specified a </newline>
> target directory for the file to be moved to. </br>
> h. in the case of (g), there must also be held in the segmented directory a script labeled 'setup.sh' which prepares </newline>
> the filesystem for the presence of this file. </br>
> i. The OTAU also needs to have the ability to flash the STM EPS firmware by pulling high boot (0 or 1) and the reset pin. </br>
> i.a. should the OTAU trigger the StateMachine to prepare for an interrupt of data coming from the stm? </br>
> i.b. if the OTAU is to trigger the StateMachine, does it have to do so directly or may it do so by alerting the stm and </newline>
> the stm then triggers the StateMachine/EpsUart? </br>

## Notes

Comments: </br>
- Should probably have a designated directory for the updater to perform its operations. 
    - Lets say '/updater_dir'
    - There should be a consideration that memory is not stored on reboot 
        - in the case of a reboot any note of pending updates on audit should be held
            - lets call this file '/updater_dir/tracking.txt'
            - this file should be updated on each audit 
        - Lets say we have a binary we would like to attempt to update something with or to. 
            - lets say its location is in '/updater_dir/bin/'
            
        - Lets say we have a large file object that is being built up from segmented file transfers
            - lets say its location is in '/updater_dir/file_0x/'
        - The file object should be segmented into n-byte segments
            - This can be done in a fashion similar to the following:
              $ split -b 524288 text.bin --additional-suffix=.seg segment
           - maybe it should have an index of all the files needed and all the files stored as well as the .md5 checksum
           - lets say this exists at '/updater_dir/file_0x/xxxx/index.txt' 
   *    - Lastly and most importantly, there should be existent in the segmented directory a script that is to be run on 
           setup stage of the target update file. This is the most crucial step as it will be the step which adjusts the 
           filesystem run-time to account for the new binary. This file should most likely be named 'setup.sh' and should 
           be run only after the file has been created from the segments and the checksum has been verified. This file 
           needn't exist in the segmented directory during the storing of the segments; but should be placed there before 
           the file is moved to its target location.
   * Target Operations: 
        - Audit locally stored segmented file transfers  
        - Move files to target locations
        - Trigger calls to the file recycler to clean up directories
   * Potential Implementation Scheme: 
        - (i) Take target file on groundstation uplink size. 
        - (ii) Break the file into n-byte segments and store them (along with md5 cs)
                in local storage under '{}/uplink_target/filexxxx/segmentxxxx'
          - (a) This can be done with the 'split' command in linux
            - Should probably create a set index_src.txt file that contains the names of all the segments
          - (b) The md5 checksum can be generated with the 'md5sum' command in linux
        - (iii) Uplink the file segments to the satellite using the gds interface 
        - (iv) On the satellite, store the segments in local storage under '{}/uplink_target/filexxxx/segmentxxxx'
        - (v) Audit the segments and if all are present, concatenate them into the target file
        - (vi) Evaluate the md5 checksum of the target file and compare it to the uplinked checksum
        - (vii) If the checksums match, move the target file to the target location
"""


"""

   * Peripheral: 
      - Directory should not exist unless a manually uplinked file has been placed there. 
      - This also means that a commanding entity should have knowledge about how and what kind of information 
       is stored in the directory.
   *  - Some parameters to keep in mind: 
   *    - File segment size 
   *    - Target content file size
   *    - MD5 checksum of the target content file
   *    - Index file of all the segments which are had 
   *    - How much data is missing from the target threshold
   * > Example Entry: 
   *  '''
   *  segment_size = 64         # (n_bytes)
   *  target_size = 1024        # (n_bytes)
   *  md5_checksum = 0x12345678 # (hex) ** should be a designated file? ** consider providing path instead of value 
   *  index_file = 'index.txt'  # (string) ** self ** 
   *  missing_data = 0          # (n_bytes) ** n_target_bytes - n_segments * segment_size ** 
   *  segment_count = 16        # (n_segments) ** n_target_bytes / segment_size **
   *  target_file_type = 'bin'  # (string) ** self **
   *  '''
   * > Example File Segment Directory: 
   *   < -[updater_dir]- 
   *     < -[file_0x]- 
   *        > -[summary.txt]-
   *        > -[index.txt]- 
   *        > -[cs.md5]-
   *        > -[setup.sh]-
   *        > -[segment_a]-
   *        > -[segment_b]-
   *        > -[..]-
   *     < -[end-dir]- 
   * > Two files should always exit in a target segment directory. [ index.txt, cs.md5 ]
"""


""" 
- Parse the contents of the index file.
    * The index file should have parameter values of such: 
        - segment_size = 64         # (n_bytes)
        - target_size = 1024        # (n_bytes)
        - n_segments = 16           # (n_segments)
        - n_bytes_remaining = 0     # (n_bytes)
        - target_file_type = 'bin'  # (string)
        - concat_wildcard = 'xxx'   # (string) </br> 
"""