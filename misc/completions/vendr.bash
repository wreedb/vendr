# vim:ft=bash
_vendr_cli_name="vendr"

_vendr_complete()
{
    local cur prev flags _files _toml_path names i word

    # COMPREPLY (arr): possible completions
    # COMP_WORDS (arr): words on current cmdline
    # COMP_CWORD (index): current word being completed
    
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"

    _toml_path="vendr.toml"

    for i in "${!COMP_WORDS[@]}"; do
        word="${COMP_WORDS[i]}"

        if [[ "${word}" == "--file" || "${word}" == "-f" ]]; then
            if (( i + 1 < COMP_CWORD )) && [[ -n "${COMP_WORDS[i+1]}" ]]; then
                _toml_path="${COMP_WORDS[i+1]}"
                break
            fi
        fi
    done


    flags="-h --help -V --version -v --verbose -f --file -n --name -w --overwrite -m --manual"

    case "${prev}" in
        --file|-f)
            local _files="$(command ls *.toml */*.toml 2>/dev/null)"
            COMPREPLY=( $(compgen -W "${_files}" -- "${cur}") )
            return 0
            ;;
        --name|-n)
            local names="$(grep '^name =' "${_toml_path}" 2>/dev/null | cut -d'"' -f2 | tr '\n' ' ')"
            if [ -z "$names" ]; then
                names="<no names found in $_toml_path>"
            fi
            COMPREPLY=( $(compgen -W "$names" -- "$cur") )
            return 0
            ;;
        *) ;;
    esac

    if [[ "$cur" == -* ]]; then
        COMPREPLY=( $(compgen -W "$flags" -- "$cur") )
        return 0
    fi
}

complete -o nospace -o default -F _vendr_complete $_vendr_cli_name