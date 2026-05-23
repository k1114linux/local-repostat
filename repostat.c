#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fnmatch.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#define _rs_max_ignore 4096
#define _rs_max_attr 2048
#define _rs_max_langs 256
#define _rs_max_path 4096
#define _rs_max_line 8192

#define _rs_reset "\033[0m"
#define _rs_bold "\033[1m"
#define _rs_dim "\033[2m"

struct _rs_lang_entry {
    const char* ext;
    const char* name;
    const char* color;
    const char* hex;
};

static const struct _rs_lang_entry _rs_lang_db[] = {
    {".c",          "C",              "\033[38;2;85;85;85m",    "#555555"},
    {".h",          "C/C++ Header",   "\033[38;2;244;68;62m",   "#f44336"},
    {".cpp",        "C++",            "\033[38;2;243;75;125m",  "#f34b7d"},
    {".cc",         "C++",            "\033[38;2;243;75;125m",  "#f34b7d"},
    {".cxx",        "C++",            "\033[38;2;243;75;125m",  "#f34b7d"},
    {".hpp",        "C++",            "\033[38;2;243;75;125m",  "#f34b7d"},
    {".hxx",        "C++",            "\033[38;2;243;75;125m",  "#f34b7d"},

    {".rs",         "Rust",           "\033[38;2;222;165;132m", "#dea584"},
    {".go",         "Go",             "\033[38;2;0;173;216m",   "#00ADD8"},
    {".zig",        "Zig",            "\033[38;2;236;145;92m",  "#ec915c"},
    {".v",          "V",              "\033[38;2;76;195;217m",  "#4cc3d9"},
    {".odin",       "Odin",           "\033[38;2;56;114;186m",  "#3872ba"},
    {".d",          "D",              "\033[38;2;186;89;94m",   "#ba595e"},
    {".nim",        "Nim",            "\033[38;2;255;192;0m",   "#ffc200"},
    {".jai",        "Jai",            "\033[38;2;255;120;0m",   "#ff7800"},

    {".py",         "Python",         "\033[38;2;53;114;165m",  "#3572A5"},
    {".pyw",        "Python",         "\033[38;2;53;114;165m",  "#3572A5"},
    {".pyx",        "Cython",         "\033[38;2;255;212;59m",  "#ffd43b"},
    {".pxd",        "Cython",         "\033[38;2;255;212;59m",  "#ffd43b"},

    {".js",         "JavaScript",     "\033[38;2;241;224;90m",  "#f1e05a"},
    {".mjs",        "JavaScript",     "\033[38;2;241;224;90m",  "#f1e05a"},
    {".cjs",        "JavaScript",     "\033[38;2;241;224;90m",  "#f1e05a"},
    {".jsx",        "JavaScript",     "\033[38;2;241;224;90m",  "#f1e05a"},

    {".ts",         "TypeScript",     "\033[38;2;49;120;198m",  "#3178c6"},
    {".tsx",        "TypeScript",     "\033[38;2;49;120;198m",  "#3178c6"},

    {".java",       "Java",           "\033[38;2;176;114;25m",  "#b07219"},
    {".kt",         "Kotlin",         "\033[38;2;127;82;255m",  "#7F52FF"},
    {".kts",        "Kotlin",         "\033[38;2;127;82;255m",  "#7F52FF"},
    {".scala",      "Scala",          "\033[38;2;194;45;64m",   "#c22d40"},
    {".groovy",     "Groovy",         "\033[38;2;66;152;181m",  "#4298b5"},
    {".gradle",     "Groovy",         "\033[38;2;66;152;181m",  "#4298b5"},

    {".cs",         "C#",             "\033[38;2;35;145;32m",   "#239120"},
    {".fs",         "F#",             "\033[38;2;184;69;252m",  "#b845fc"},
    {".fsx",        "F#",             "\033[38;2;184;69;252m",  "#b845fc"},
    {".fsi",        "F#",             "\033[38;2;184;69;252m",  "#b845fc"},

    {".swift",      "Swift",          "\033[38;2;240;80;50m",   "#F05138"},
    {".m",          "Objective-C",    "\033[38;2;67;142;255m",  "#438eff"},
    {".mm",         "Objective-C++",  "\033[38;2;104;104;104m", "#686868"},

    {".php",        "PHP",            "\033[38;2;79;93;149m",   "#4F5D95"},
    {".rb",         "Ruby",           "\033[38;2;112;21;22m",   "#701516"},
    {".erb",        "ERB",            "\033[38;2;112;21;22m",   "#701516"},

    {".sh",         "Shell",          "\033[38;2;137;224;81m",  "#89e051"},
    {".bash",       "Shell",          "\033[38;2;137;224;81m",  "#89e051"},
    {".zsh",        "Shell",          "\033[38;2;137;224;81m",  "#89e051"},
    {".fish",       "Fish",           "\033[38;2;76;195;217m",  "#4cc3d9"},
    {".nu",         "Nushell",        "\033[38;2;76;175;80m",   "#4caf50"},
    {".ps1",        "PowerShell",     "\033[38;2;1;36;86m",     "#012456"},

    {".html",       "HTML",           "\033[38;2;227;76;38m",   "#e34c26"},
    {".css",        "CSS",            "\033[38;2;86;61;124m",   "#563d7c"},
    {".scss",       "SCSS",           "\033[38;2;198;83;140m",  "#c6538c"},
    {".sass",       "Sass",           "\033[38;2;165;91;128m",  "#a55b80"},
    {".less",       "Less",           "\033[38;2;29;54;93m",    "#1d365d"},

    {".vue",        "Vue",            "\033[38;2;65;184;131m",  "#41b883"},
    {".svelte",     "Svelte",         "\033[38;2;255;62;0m",    "#ff3e00"},
    {".astro",      "Astro",          "\033[38;2;255;93;0m",    "#ff5d00"},

    {".json",       "JSON",           "\033[38;2;41;128;185m",  "#292929"},
    {".json5",      "JSON5",          "\033[38;2;38;139;210m",  "#268bd2"},
    {".yaml",       "YAML",           "\033[38;2;203;76;22m",   "#cb4b16"},
    {".yml",        "YAML",           "\033[38;2;203;76;22m",   "#cb4b16"},
    {".toml",       "TOML",           "\033[38;2;159;50;50m",   "#9c4221"},
    {".xml",        "XML",            "\033[38;2;0;115;207m",   "#0060ac"},
    {".ini",        "INI",            "\033[38;2;106;115;125m", "#6a737d"},
    {".conf",       "Config",         "\033[38;2;106;115;125m", "#6a737d"},
    {".cfg",        "Config",         "\033[38;2;106;115;125m", "#6a737d"},

    {".md",         "Markdown",       "\033[38;2;8;63;161m",    "#083fa1"},
    {".rst",        "reStructuredText","\033[38;2;20;122;80m",  "#147a50"},
    {".tex",        "TeX",            "\033[38;2;61;97;55m",    "#3D6117"},
    {".typ",        "Typst",          "\033[38;2;35;35;35m",    "#222222"},

    {".sql",        "SQL",            "\033[38;2;232;88;0m",    "#e38c00"},
    {".graphql",    "GraphQL",        "\033[38;2;225;0;152m",   "#e10098"},
    {".gql",        "GraphQL",        "\033[38;2;225;0;152m",   "#e10098"},

    {".lua",        "Lua",            "\033[38;2;0;0;128m",     "#000080"},
    {".r",          "R",              "\033[38;2;25;140;188m",  "#198CE7"},
    {".jl",         "Julia",          "\033[38;2;162;112;186m", "#a270ba"},
    {".hs",         "Haskell",        "\033[38;2;94;80;134m",   "#5e5086"},
    {".lhs",        "Haskell",        "\033[38;2;94;80;134m",   "#5e5086"},
    {".clj",        "Clojure",        "\033[38;2;219;88;85m",   "#db5855"},
    {".cljs",       "ClojureScript",  "\033[38;2;219;88;85m",   "#db5855"},
    {".el",         "Emacs Lisp",     "\033[38;2;199;146;234m", "#c792ea"},
    {".lisp",       "Common Lisp",    "\033[38;2;63;182;131m",  "#3fb283"},
    {".scm",        "Scheme",         "\033[38;2;30;74;236m",   "#1e4aec"},
    {".rkt",        "Racket",         "\033[38;2;34;148;162m",  "#22949e"},

    {".erl",        "Erlang",         "\033[38;2;184;57;152m",  "#B83998"},
    {".ex",         "Elixir",         "\033[38;2;110;74;126m",  "#6e4a7e"},
    {".exs",        "Elixir",         "\033[38;2;110;74;126m",  "#6e4a7e"},
    {".gleam",      "Gleam",          "\033[38;2;255;175;243m", "#ffaff3"},

    {".dart",       "Dart",           "\033[38;2;0;180;171m",   "#00B4AB"},
    {".qml",        "QML",            "\033[38;2;68;204;17m",   "#44cc11"},
    {".slint",      "Slint",          "\033[38;2;35;140;245m",  "#238cf5"},

    {".dockerignore","Docker Ignore", "\033[38;2;56;77;84m",    "#384d54"},
    {".tf",         "Terraform",      "\033[38;2;92;78;229m",   "#5C4EE5"},
    {".hcl",        "HCL",            "\033[38;2;92;78;229m",   "#5C4EE5"},

    {".proto",      "Protocol Buffers","\033[38;2;0;150;136m",  "#009688"},
    {".wasm",       "WebAssembly",    "\033[38;2;101;100;255m", "#654ff0"},
    {".wgsl",       "WGSL",           "\033[38;2;0;200;255m",   "#00c8ff"},

    {".sol",        "Solidity",       "\033[38;2;54;54;54m",    "#363636"},
    {".move",       "Move",           "\033[38;2;76;111;255m",  "#4c6fff"},

    {".http",       "HTTP",           "\033[38;2;0;170;255m",   "#00aaff"},
    {".hurl",       "Hurl",           "\033[38;2;255;120;0m",   "#ff7800"},

    {".kdl",        "KDL",            "\033[38;2;255;140;0m",   "#ff8c00"},
    {".ron",        "RON",            "\033[38;2;176;114;25m",  "#b07219"},
    {".cue",        "Cue",            "\033[38;2;88;166;255m",  "#58a6ff"},
    {".rego",       "Rego",           "\033[38;2;200;16;46m",   "#c8102e"},
    {".templ",      "Templ",          "\033[38;2;0;150;136m",   "#009688"},

    {".asm",        "Assembly",       "\033[38;2;110;76;190m",  "#6E4BBE"},
    {".s",          "Assembly",       "\033[38;2;110;76;190m",  "#6E4BBE"},
    {".S",          "Assembly",       "\033[38;2;110;76;190m",  "#6E4BBE"},

    {".f",          "Fortran",        "\033[38;2;77;111;76m",   "#4d6f4c"},
    {".f90",        "Fortran",        "\033[38;2;77;111;76m",   "#4d6f4c"},
    {".f95",        "Fortran",        "\033[38;2;77;111;76m",   "#4d6f4c"},

    {".ada",        "Ada",            "\033[38;2;2;57;140m",    "#02398C"},
    {".adb",        "Ada",            "\033[38;2;2;57;140m",    "#02398C"},
    {".ads",        "Ada",            "\033[38;2;2;57;140m",    "#02398C"},

    {".pas",        "Pascal",         "\033[38;2;215;166;0m",   "#d7a600"},
    {".pp",         "Pascal",         "\033[38;2;215;166;0m",   "#d7a600"},

    {".vb",         "Visual Basic",   "\033[38;2;148;93;183m",  "#945db7"},
    {".bat",        "Batchfile",      "\033[38;2;193;154;107m", "#C1A26B"},
    {".cmd",        "Batchfile",      "\033[38;2;193;154;107m", "#C1A26B"},

    {".ahk",        "AutoHotkey",     "\033[38;2;101;123;175m", "#6594b9"},
    {".awk",        "AWK",            "\033[38;2;160;100;50m",  "#a06432"},

    {".yaml.j2",    "Jinja",          "\033[38;2;184;134;11m",  "#b8860b"},
    {".jinja",      "Jinja",          "\033[38;2;184;134;11m",  "#b8860b"},
    {".jinja2",     "Jinja",          "\033[38;2;184;134;11m",  "#b8860b"},

    {((void*)0), ((void*)0), ((void*)0), ((void*)0)}
};

struct _rs_ignore_pat {
    char pattern[512];
    char prefix[512];

    int negate;
    int dir_only;
    int anchored;
};

struct _rs_attr_pat {
    char pattern[512];
    char language[128];

    int generated;
    int vendored;
    int documentation;
    int detectable;
};

struct _rs_lang_stat {
    char name[128];
    char color[64];
    char hex[16];

    long long bytes;
    long long lines;
};

static struct _rs_ignore_pat _rs_ignore[_rs_max_ignore];
static int _rs_ignore_count = 0;

static struct _rs_attr_pat _rs_attr[_rs_max_attr];
static int _rs_attr_count = 0;

static struct _rs_lang_stat _rs_langs[_rs_max_langs];
static int _rs_lang_count = 0;

static long long _rs_total_bytes = 0;
static long long _rs_total_lines = 0;
static long long _rs_total_files = 0;
static long long _rs_skipped = 0;

static char _rs_root[_rs_max_path];

static void _rs_str_lower(char* s_ptr) { for(; *s_ptr; s_ptr++) *s_ptr = (char)tolower((unsigned char)*s_ptr); }

static const char* _rs_file_ext(const char* path_ptr) {
    const char* base_ptr = strrchr(path_ptr, '/');
    base_ptr = base_ptr ? base_ptr+1 : path_ptr;

    static const struct { const char* name; const char* ext; } _specials[] = { /* SPECIALS */
        {"Makefile",        ".makefile"}, 
        {"makefile",    ".makefile"},
        {"GNUmakefile",     ".makefile"},
        {"Dockerfile",  ".dockerfile"},
        {"dockerfile",      ".dockerfile"}, 
        {"CMakeLists.txt", ".cmake"},
        {((void*)0), ((void*)0)}
    };
    for(int i = 0; _specials[i].name; i++) { if(strcmp(base_ptr, _specials[i].name) == 0) return(_specials[i].ext); }

    const char* dot_ptr = strrchr(base_ptr, '.');
    if(!dot_ptr || dot_ptr == base_ptr) return("");

    return(dot_ptr);
}

static const struct _rs_lang_entry* _rs_lang_for_ext(const char* ext_ptr) {
    char ext[64];
    strncpy(ext, ext_ptr, sizeof(ext)-1); ext[sizeof(ext)-1] = '\0';
    _rs_str_lower(ext);
    for(int i = 0; _rs_lang_db[i].ext; i++) {
        if(strcmp(_rs_lang_db[i].ext, ext) == 0) return(&_rs_lang_db[i]);
    } return(((void*)0));
}

static struct _rs_lang_stat* _rs_get_or_create_lang(const char* name_ptr, const char* color_ptr, const char* hex_ptr) {
    for(int i = 0; i < _rs_lang_count; i++) {
        if(strcmp(_rs_langs[i].name, name_ptr) == 0) return(&_rs_langs[i]);
    }
    if(_rs_lang_count >= _rs_max_langs) return(((void*)0));
    struct _rs_lang_stat* ls_ptr = &_rs_langs[_rs_lang_count++];
    strncpy(ls_ptr->name, name_ptr, sizeof(ls_ptr->name)-1);
    strncpy(ls_ptr->color, color_ptr, sizeof(ls_ptr->color)-1);
    strncpy(ls_ptr->hex, hex_ptr, sizeof(ls_ptr->hex)-1);
    ls_ptr->bytes = ls_ptr->lines = 0;
    return(ls_ptr);
}

static void _rs_trim_trailing(char* s_ptr) {
    int len = (int)strlen(s_ptr);
    while(len > 0 && s_ptr[len-1] == ' ') {
        if(len > 1 && s_ptr[len-2] == '\\') break;
        s_ptr[--len] = '\0';
    }
}

static void _rs_load_gitignore(const char* dir_ptr) {
    char path[_rs_max_path];
    snprintf(path, sizeof(path), "%s/.gitignore", dir_ptr);
    FILE* f_ptr = fopen(path, "r");
    if(!f_ptr) return;

    char line[512];
    while(fgets(line, sizeof(line), f_ptr)) {
        int len = (int)strlen(line);
        while(len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';
        _rs_trim_trailing(line);
        if(len == 0 || line[0] == '#') continue;
        if(_rs_ignore_count >= _rs_max_ignore) break;

        struct _rs_ignore_pat* p_ptr = &_rs_ignore[_rs_ignore_count++];
        memset(p_ptr, 0, sizeof(*p_ptr));
        char* pat_ptr = line;

        if(*pat_ptr == '!') { p_ptr->negate = 1; pat_ptr++; }
        else if(*pat_ptr == '\\' && (pat_ptr[1] == '#' || pat_ptr[1] == '!')) pat_ptr++;

        int plen = (int)strlen(pat_ptr);
        if(plen > 0 && pat_ptr[plen-1] == '/') { p_ptr->dir_only = 1; pat_ptr[--plen] = '\0'; }
        if(strchr(pat_ptr, '/')) p_ptr->anchored = 1;
        if(*pat_ptr == '/') { p_ptr->anchored = 1; pat_ptr++; }

        snprintf(p_ptr->pattern, sizeof(p_ptr->pattern), "%s", pat_ptr); /* WARNING */

        const char* rel_ptr = dir_ptr + strlen(_rs_root);
        if(*rel_ptr == '/') rel_ptr++;
        strncpy(p_ptr->prefix, rel_ptr, sizeof(p_ptr->prefix)-1);
    }
    fclose(f_ptr);
}

static void _rs_load_gitattributes(const char* dir_ptr) {
    char path[_rs_max_path];
    snprintf(path, sizeof(path), "%s/.gitattributes", dir_ptr);
    FILE* f_ptr = fopen(path, "r");
    if(!f_ptr) return;

    char line[_rs_max_line];
    while(fgets(line, sizeof(line), f_ptr)) {
        int len = (int)strlen(line);
        while(len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';
        if(len == 0 || line[0] == '#') continue;
        if(_rs_attr_count >= _rs_max_attr) break;

        struct _rs_attr_pat* ap_ptr = &_rs_attr[_rs_attr_count];
        memset(ap_ptr, 0, sizeof(*ap_ptr));
        ap_ptr->detectable = 1;

        char* tok_ptr = strtok(line, " \t");
        if(!tok_ptr) continue;
        strncpy(ap_ptr->pattern, tok_ptr, sizeof(ap_ptr->pattern)-1);

        while((tok_ptr = strtok(((void*)0), " \t")) != ((void*)0)) {
            if(strncmp(tok_ptr, "linguist-language=", 18) == 0)
                strncpy(ap_ptr->language, tok_ptr+18, sizeof(ap_ptr->language)-1);
            else if(strcmp(tok_ptr, "linguist-generated") == 0 || strcmp(tok_ptr, "linguist-generated=true") == 0)
                ap_ptr->generated = 1;
            else if(strcmp(tok_ptr, "linguist-vendored") == 0 || strcmp(tok_ptr, "linguist-vendored=true") == 0)
                ap_ptr->vendored = 1;
            else if(strcmp(tok_ptr, "linguist-documentation") == 0 || strcmp(tok_ptr, "linguist-documentation=true") == 0)
                ap_ptr->documentation = 1;
            else if(strcmp(tok_ptr, "linguist-detectable=false") == 0)
                ap_ptr->detectable = 0;
        }
        _rs_attr_count++;
    }
    fclose(f_ptr);
}

static const char* _rs_rel(const char* abs_ptr) {
    const char* r_ptr = abs_ptr + strlen(_rs_root);
    if(*r_ptr == '/') r_ptr++;
    return(r_ptr);
}

static int _rs_match_pat(const struct _rs_ignore_pat* p_ptr, const char* relp_ptr, int is_dir) {
    if(p_ptr->dir_only && !is_dir) return(0);
    const char* check_ptr = relp_ptr;
    if(p_ptr->prefix[0]) {
        size_t plen = strlen(p_ptr->prefix);
        if(strncmp(relp_ptr, p_ptr->prefix, plen) != 0) return(0);
        check_ptr = relp_ptr + plen;
        if(*check_ptr == '/') check_ptr++;
    }
    if(p_ptr->anchored) {
        if(fnmatch(p_ptr->pattern, check_ptr, FNM_PATHNAME|FNM_PERIOD) == 0) return(1);
    } else {
        const char* base_ptr = strrchr(check_ptr, '/');
        base_ptr = base_ptr ? base_ptr+1 : check_ptr;
        if(fnmatch(p_ptr->pattern, base_ptr, FNM_PERIOD) == 0) return(1);
        if(fnmatch(p_ptr->pattern, check_ptr, FNM_PERIOD) == 0) return(1);
    }
    return(0);
}

static int _rs_is_ignored(const char* abs_ptr, int is_dir) {
    const char* relp_ptr = _rs_rel(abs_ptr);
    int ignored = 0;
    for(int i = 0; i < _rs_ignore_count; i++) {
        if(_rs_match_pat(&_rs_ignore[i], relp_ptr, is_dir))
            ignored = _rs_ignore[i].negate ? 0 : 1;
    }
    return(ignored);
}

struct _rs_attr_result { const char* language_ptr; int skip; };

static struct _rs_attr_result _rs_apply_attrs(const char* abs_ptr) {
    struct _rs_attr_result r = {((void*)0), 0};
    const char* relp_ptr = _rs_rel(abs_ptr);
    const char* base_ptr = strrchr(relp_ptr, '/');
    base_ptr = base_ptr ? base_ptr+1 : relp_ptr;
    for(int i = 0; i < _rs_attr_count; i++) {
        struct _rs_attr_pat* ap_ptr = &_rs_attr[i];
        if(fnmatch(ap_ptr->pattern, base_ptr, FNM_PERIOD) == 0 ||
           fnmatch(ap_ptr->pattern, relp_ptr, FNM_PATHNAME|FNM_PERIOD) == 0) {
            if(ap_ptr->generated || ap_ptr->vendored || ap_ptr->documentation || !ap_ptr->detectable)
                r.skip = 1;
            if(ap_ptr->language[0]) r.language_ptr = ap_ptr->language;
        }
    }
    return(r);
}

static long long _rs_count_lines(const char* path_ptr) {
    FILE* f_ptr = fopen(path_ptr, "rb");
    if(!f_ptr) return(0);

    long long lines = 0;
    char buf[65536]; size_t n;
    int checked = 0;

    while((n = fread(buf, 1, sizeof(buf), f_ptr)) > 0) {
        if(!checked) {
            size_t chk = n < 8192 ? n : 8192;
            for(size_t i = 0; i < chk; i++) {
                if(buf[i] == '\0') { fclose(f_ptr); return(-1); }
            }
            checked = 1;
        }
        for(size_t i = 0; i < n; i++) { if(buf[i] == '\n') lines++; }
    }
    if(checked) lines++;
    fclose(f_ptr); return(lines);
}

static const char* _rs_vendor_paths[] = {
    "vendor/", "node_modules/", "bower_components/", "third_party/",
    "thirdparty/", "3rdparty/", "extern/", "external/", "deps/",
    "dependencies/", ".git/", ((void*)0)
};

static int _rs_is_vendor(const char* relp_ptr) {
    for(int i = 0; _rs_vendor_paths[i]; i++) {
        if(strstr(relp_ptr, _rs_vendor_paths[i])) return(1);
    }
    return(0);
}

static void _rs_walk(const char* dir_ptr) {
    DIR* d_ptr = opendir(dir_ptr);
    if(!d_ptr) return;

    _rs_load_gitignore(dir_ptr);
    _rs_load_gitattributes(dir_ptr);

    struct dirent* ent_ptr;
    while((ent_ptr = readdir(d_ptr)) != ((void*)0)) {
        if(strcmp(ent_ptr->d_name, ".") == 0 || strcmp(ent_ptr->d_name, "..") == 0) continue;

        char full[_rs_max_path];
        snprintf(full, sizeof(full), "%s/%s", dir_ptr, ent_ptr->d_name);

        struct stat st;
        if(lstat(full, &st) != 0) continue;

        int is_dir = S_ISDIR(st.st_mode);
        int is_file = S_ISREG(st.st_mode);

        if(_rs_is_ignored(full, is_dir)) { _rs_skipped++; continue; }

        if(is_dir) { _rs_walk(full); continue; }
        if(!is_file) continue;

        const char* relp_ptr = _rs_rel(full);
        if(_rs_is_vendor(relp_ptr)) { _rs_skipped++; continue; }

        struct _rs_attr_result ar = _rs_apply_attrs(full);
        if(ar.skip) { _rs_skipped++; continue; }

        const char* lang_name_ptr = ((void*)0);
        const char* lang_color_ptr = "\033[38;2;150;150;150m";
        const char* lang_hex_ptr = "#969696";

        if(ar.language_ptr) {
            lang_name_ptr = ar.language_ptr;
            for(int i = 0; _rs_lang_db[i].ext; i++) {
                if(strcasecmp(_rs_lang_db[i].name, ar.language_ptr) == 0) {
                    lang_color_ptr = _rs_lang_db[i].color;
                    lang_hex_ptr = _rs_lang_db[i].hex; break;
                }
            }
        } else {
            const struct _rs_lang_entry* le_ptr = _rs_lang_for_ext(_rs_file_ext(full));
            if(le_ptr) {
                lang_name_ptr = le_ptr->name;
                lang_color_ptr = le_ptr->color;
                lang_hex_ptr = le_ptr->hex;
            }
        }

        long long lines = _rs_count_lines(full);
        if(lines < 0) { _rs_skipped++; continue; }

        long long bytes = (long long)st.st_size;
        _rs_total_bytes += bytes; _rs_total_lines += lines; _rs_total_files++;

        const char* bucket_ptr = lang_name_ptr ? lang_name_ptr : "Other";
        const char* bcol_ptr = lang_name_ptr ? lang_color_ptr : "\033[38;2;130;130;130m";
        const char* bhex_ptr = lang_name_ptr ? lang_hex_ptr : "#828282";

        struct _rs_lang_stat* ls_ptr = _rs_get_or_create_lang(bucket_ptr, bcol_ptr, bhex_ptr);
        if(ls_ptr) { ls_ptr->bytes += bytes; ls_ptr->lines += lines; }
    }
    closedir(d_ptr);
}

static void _rs_fmt_bytes(long long b, char* out_ptr, size_t sz) {
    if(b < 1024) snprintf(out_ptr, sz, "%lld B", b);
    else if(b < 1024*1024) snprintf(out_ptr, sz, "%.1f KB", b/1024.0);
    else if(b < 1024LL*1024*1024) snprintf(out_ptr, sz, "%.2f MB", b/(1024.0*1024));
    else snprintf(out_ptr, sz, "%.2f GB", b/(1024.0*1024*1024));
}

static void _rs_fmt_num(long long n, char* out_ptr, size_t sz) {
    char tmp[32]; snprintf(tmp, sizeof(tmp), "%lld", n);
    int len = (int)strlen(tmp), dest = 0;
    int comma = len%3; if(comma == 0) comma = 3;
    for(int i = 0; i < len && dest < (int)sz-1; i++) {
        if(i == comma && i != len-1) { out_ptr[dest++] = ','; comma += 3; }
        out_ptr[dest++] = tmp[i];
    }
    out_ptr[dest] = '\0';
}

static int _rs_cmp_lang(const void* a_ptr, const void* b_ptr) {
    const struct _rs_lang_stat* la_ptr = (const struct _rs_lang_stat*)a_ptr;
    const struct _rs_lang_stat* lb_ptr = (const struct _rs_lang_stat*)b_ptr;
    if(lb_ptr->bytes > la_ptr->bytes) return( 1);
    if(lb_ptr->bytes < la_ptr->bytes) return(-1);
    return(0);
}

static int _rs_term_width(void) {
    const char* cols_ptr = getenv("COLUMNS");
    if(cols_ptr) { int w = atoi(cols_ptr); if(w > 20) return(w); }
#ifdef TIOCGWINSZ
    struct winsize ws;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) return(ws.ws_col);
#endif
    return(80);
}

static void _rs_print_bar(int width) {
    if(_rs_total_bytes == 0) return;
    qsort(_rs_langs, _rs_lang_count, sizeof(struct _rs_lang_stat), _rs_cmp_lang);

    int* seg_ptr = (int*)calloc(_rs_lang_count, sizeof(int));
    if(!seg_ptr) return;

    int used = 0;
    for(int i = 0; i < _rs_lang_count; i++) {
        double frac = (double)_rs_langs[i].bytes / (double)_rs_total_bytes;
        seg_ptr[i] = (int)(frac * width + 0.5);
        used += seg_ptr[i];
    }
    if(used != width && _rs_lang_count > 0) seg_ptr[0] += (width - used);

    printf("  ");
    for(int i = 0; i < _rs_lang_count; i++) {
        if(seg_ptr[i] <= 0) continue;
        char bg[64]; strncpy(bg, _rs_langs[i].color, sizeof(bg)-1);
        char* p_ptr = strstr(bg, "38;"); if(p_ptr) p_ptr[0] = '4';
        printf("%s", bg);
        for(int j = 0; j < seg_ptr[i]; j++) printf(" ");
        printf(_rs_reset);
    }
    printf("\n");
    free(seg_ptr);
}

static void _rs_print_legend(void) {
    int width = _rs_term_width();
    int col_width = (width - 4) / 2; if(col_width < 30) col_width = 30;
    int half = (_rs_lang_count + 1) / 2;

    for(int i = 0; i < half; i++) {
        struct _rs_lang_stat* l_ptr = &_rs_langs[i];
        double pct = _rs_total_bytes > 0 ? 100.0 * l_ptr->bytes / _rs_total_bytes : 0.0;
        char sz[32], lns[32];
        _rs_fmt_bytes(l_ptr->bytes, sz, sizeof(sz));
        _rs_fmt_num(l_ptr->lines, lns, sizeof(lns));

        printf("  %s●" _rs_reset " " _rs_bold "%s" _rs_reset, l_ptr->color, l_ptr->name);
        int printed = 3 + (int)strlen(l_ptr->name);
        char info[128]; snprintf(info, sizeof(info), "  %.1f%%  %s lines  %s", pct, lns, sz);
        printf(_rs_dim "%s" _rs_reset, info);
        printed += (int)strlen(info);

        int right = i + half;
        if(right < _rs_lang_count) {
            struct _rs_lang_stat* r_ptr = &_rs_langs[right];
            double rpct = _rs_total_bytes > 0 ? 100.0 * r_ptr->bytes / _rs_total_bytes : 0.0;
            _rs_fmt_bytes(r_ptr->bytes, sz, sizeof(sz));
            _rs_fmt_num(r_ptr->lines, lns, sizeof(lns));
            int pad = col_width - printed + 2; if(pad < 2) pad = 2;
            for(int k = 0; k < pad; k++) printf(" ");
            printf("%s●" _rs_reset " " _rs_bold "%s" _rs_reset, r_ptr->color, r_ptr->name);
            char rinfo[128]; snprintf(rinfo, sizeof(rinfo), "  %.1f%%  %s lines  %s", rpct, lns, sz);
            printf(_rs_dim "%s" _rs_reset, rinfo);
        }
        if(i+1 >= half) continue;
        printf("\n");
    }
}

static void _rs_print_summary(void) {
    char total_sz[32], total_lns[32], total_fs[32], skipped[32];
    _rs_fmt_bytes(_rs_total_bytes, total_sz, sizeof(total_sz));
    _rs_fmt_num(_rs_total_lines, total_lns, sizeof(total_lns));
    _rs_fmt_num(_rs_total_files, total_fs, sizeof(total_fs));
    _rs_fmt_num(_rs_skipped, skipped, sizeof(skipped));
    printf("  " _rs_bold "Size" _rs_reset "          " _rs_dim "%s" _rs_reset "\n", total_sz);
    printf("  " _rs_bold "Lines of code" _rs_reset " " _rs_dim "%s" _rs_reset "\n", total_lns);
    printf("  " _rs_bold "Files" _rs_reset "         " _rs_dim "%s" _rs_reset "\n", total_fs);
    printf("  " _rs_bold "Languages" _rs_reset "     " _rs_dim "%d" _rs_reset "\n", _rs_lang_count);
    if(_rs_skipped > 0)
        printf("  " _rs_bold "Skipped" _rs_reset "       " _rs_dim "%s (binary/vendor/ignored)" _rs_reset "\n", skipped);
    printf("\n");
}

int main(int argc, char* argv_ptr[]) {
    const char* target_ptr = (argc > 1) ? argv_ptr[1] : ".";

    if(!realpath(target_ptr, _rs_root)) {
        fprintf(stderr, "repostat: cannot resolve '%s': %s\n", target_ptr, strerror(errno));
        return(1);
    }

    struct stat st;
    if(stat(_rs_root, &st) != 0 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "repostat: '%s' is not a directory\n", _rs_root);
        return(1);
    }

    _rs_walk(_rs_root);

    if(_rs_total_files == 0) {
        printf("repostat: no source files found in '%s'\n", _rs_root);
        return(0);
    }

    qsort(_rs_langs, _rs_lang_count, sizeof(struct _rs_lang_stat), _rs_cmp_lang);

    int width = _rs_term_width();
    int bar_width = width - 4; if(bar_width < 20) bar_width = 20;

    _rs_print_summary();

    printf("  " _rs_bold "Languages" _rs_reset "\n");
    _rs_print_bar(bar_width);
    _rs_print_legend();
    printf("  \033[38;2;80;80;80m");
    printf(_rs_reset "\n");

    return(0);
}
