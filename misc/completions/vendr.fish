# Fish completion for vendr

# Helper function to get the TOML file path from command line
function __vendr_get_toml_path
    set -l file_flag_index (contains -i -- -f $argv; or contains -i -- --file $argv)
    if test -n "$file_flag_index"
        set -l next_index (math $file_flag_index + 1)
        if test $next_index -le (count $argv)
            echo $argv[$next_index]
            return 0
        end
    end
    echo "vendr.toml"
end

# Helper function to list TOML files
function __vendr_toml_files
    ls *.toml 2>/dev/null
end

# Helper function to extract names from TOML file
function __vendr_names
    set -l toml_path (__vendr_get_toml_path (commandline -opc))
    if test -f "$toml_path"
        grep '^name =' "$toml_path" 2>/dev/null | cut -d'"' -f2
    end
end

# No argument flags
complete -c vendr -s h -l help -d "Show help information"
complete -c vendr -s V -l version -d "Show version information"
complete -c vendr -s v -l vebose -d "Display more detailed output of actions"
complete -c vendr -s w -l overwrite -d "Overwrite existing files"

# File flag with TOML file completion
complete -c vendr -s f -l file -d "Specify TOML file" -r -f -a "(__vendr_toml_files)"

# Name flag with dynamic completion based on selected file
complete -c vendr -s n -l name -d "Specify name" -r -f -a "(__vendr_names)"
