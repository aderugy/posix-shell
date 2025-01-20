status_file()
{
    FILE=$1
    if [ -e "$FILE" ]; then
        if [ -f "$FILE" ]; then
            echo "$FILE is a regular file."
                fi
                if [ -d "$FILE" ]; then
                    echo "$FILE is a directory."
                        fi
                        if [ -r "$FILE" ]; then
                            echo "$FILE is readable."
                                fi
                                if [ -w "$FILE" ]; then
                                    echo "$FILE is writable."
                                        fi
                                        if [ -x "$FILE" ]; then
                                            echo "$FILE is executable/searchable."
                                                fi
                                        else
                                            echo "$FILE does not exist"
                                                exit 1
                                                fi
}
status_file packed.sh
status_file unpacked.sh
status_file imbricated.sh
