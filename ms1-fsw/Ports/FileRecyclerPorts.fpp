module FileRecyclerModule { 

    @ Port for requesting the deletion of vertain files of a given basis 
    port FileRecyclerOpPort(
        count: U32 @< File attribute that determines which files are deleted 
        op: FileRecyclerOp
    )

    @ Port for indicating completion 
    port FileRecyclerStatusPort( 
        result: bool @< Whether or not something was completed successfully 
    )

    #@ Port for meme recycling 
    #port MemeRecyclerOpPort(
    #    op: CronOp
    #)

    @ Port for passing a directory path to be recycled 
    port CleanUpDataPort(
        path: CleanUpData @< The path to the directory to be recycled
    )

}