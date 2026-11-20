# ---------------------------------------------------------------------------
# Doxygen documentation target (CMake-driven, à la CppDocumentExample).
#
# Enabled with -DPCP_BUILD_DOCS=ON. All settings below mirror the project's
# Doxyfile starter config, expressed as DOXYGEN_* cache variables so that
# doxygen_add_docs() generates an equivalent Doxyfile at configure time.
#
# Build with:  cmake --build <build-dir> --target docs
# HTML output: <build-dir>/docs/doxygen/html/index.html
# ---------------------------------------------------------------------------

find_package(Doxygen REQUIRED dot)

# Optional PlantUML support: enable @startuml/@enduml blocks when a plantuml.jar
# is discoverable on the system (needs Java). Purely additive — skipped if absent.
find_file(PLANTUML_JARFILE
  NAMES plantuml.jar
  PATH_SUFFIXES PlantUML plantuml Plantuml
  PATHS /usr/share/ /usr/local/share/ /usr/share/plantuml /usr/local/bin
        /opt/local/share/java/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(plantuml DEFAULT_MSG PLANTUML_JARFILE)

# --- Project information -----------------------------------------------------
set(DOXYGEN_DOXYFILE_ENCODING      UTF-8)
set(DOXYGEN_PROJECT_NAME           "PointCloudPipeline")
set(DOXYGEN_PROJECT_NUMBER         "${PROJECT_VERSION}")
set(DOXYGEN_PROJECT_BRIEF
    "3D k-d tree point-cloud k-NN / obstacle-avoidance query and benchmark demo")
set(DOXYGEN_OUTPUT_DIRECTORY       "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
set(DOXYGEN_CREATE_SUBDIRS         NO)
set(DOXYGEN_OUTPUT_LANGUAGE        English)
set(DOXYGEN_BRIEF_MEMBER_DESC      YES)
set(DOXYGEN_REPEAT_BRIEF           YES)
set(DOXYGEN_ALWAYS_DETAILED_SEC    NO)
set(DOXYGEN_FULL_PATH_NAMES        YES)
set(DOXYGEN_STRIP_FROM_PATH        "${PROJECT_SOURCE_DIR}")
set(DOXYGEN_JAVADOC_AUTOBRIEF      YES)
set(DOXYGEN_QT_AUTOBRIEF           NO)
set(DOXYGEN_MULTILINE_CPP_IS_BRIEF NO)
set(DOXYGEN_TAB_SIZE               4)
set(DOXYGEN_MARKDOWN_SUPPORT       YES)
set(DOXYGEN_AUTOLINK_SUPPORT       YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT    YES)

# --- Build / extraction settings ---------------------------------------------
set(DOXYGEN_EXTRACT_ALL             NO)
set(DOXYGEN_EXTRACT_PRIVATE         NO)
set(DOXYGEN_EXTRACT_PRIV_VIRTUAL    NO)
set(DOXYGEN_EXTRACT_PACKAGE         NO)
set(DOXYGEN_EXTRACT_STATIC          NO)
set(DOXYGEN_EXTRACT_LOCAL_CLASSES   YES)
set(DOXYGEN_EXTRACT_LOCAL_METHODS   NO)
set(DOXYGEN_EXTRACT_ANON_NSPACES    NO)
set(DOXYGEN_HIDE_UNDOC_MEMBERS      NO)
set(DOXYGEN_HIDE_UNDOC_CLASSES      NO)
set(DOXYGEN_HIDE_UNDOC_RELATIONS    YES)
set(DOXYGEN_HIDE_IN_BODY_DOCS       NO)
set(DOXYGEN_INTERNAL_DOCS           NO)
set(DOXYGEN_CASE_SENSE_NAMES        YES)
set(DOXYGEN_HIDE_SCOPE_NAMES        NO)
set(DOXYGEN_SHOW_INCLUDE_FILES      YES)
set(DOXYGEN_SORT_MEMBER_DOCS        YES)
set(DOXYGEN_SORT_BRIEF_DOCS         NO)
set(DOXYGEN_SORT_GROUP_NAMES        YES)
set(DOXYGEN_GENERATE_TODOLIST       YES)
set(DOXYGEN_GENERATE_TESTLIST       YES)
set(DOXYGEN_GENERATE_BUGLIST        YES)
set(DOXYGEN_GENERATE_DEPRECATEDLIST YES)
set(DOXYGEN_MAX_INITIALIZER_LINES   30)
set(DOXYGEN_SHOW_USED_FILES         YES)
set(DOXYGEN_SHOW_FILES              YES)
set(DOXYGEN_SHOW_NAMESPACES         YES)

# --- Warnings ----------------------------------------------------------------
set(DOXYGEN_QUIET                   NO)
set(DOXYGEN_WARNINGS                YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED    YES)
set(DOXYGEN_WARN_IF_DOC_ERROR       YES)
set(DOXYGEN_WARN_NO_PARAMDOC        NO)
set(DOXYGEN_WARN_AS_ERROR           NO)
set(DOXYGEN_WARN_FORMAT             "$file:$line: $text")

# --- Input -------------------------------------------------------------------
set(DOXYGEN_INPUT_ENCODING          UTF-8)
set(DOXYGEN_FILE_PATTERNS           *.h *.hpp *.hh *.hxx *.cpp *.cc *.cxx *.inl *.dox *.md)
set(DOXYGEN_RECURSIVE               YES)
set(DOXYGEN_EXCLUDE_SYMLINKS        NO)
set(DOXYGEN_EXCLUDE_PATTERNS        */build/* */third_party/* */external/* */.git/*)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE  "${PROJECT_SOURCE_DIR}/README.md")

# --- Source browsing ---------------------------------------------------------
set(DOXYGEN_SOURCE_BROWSER          YES)
set(DOXYGEN_INLINE_SOURCES          NO)
set(DOXYGEN_STRIP_CODE_COMMENTS     YES)
set(DOXYGEN_REFERENCED_BY_RELATION  NO)
set(DOXYGEN_REFERENCES_RELATION     NO)
set(DOXYGEN_REFERENCES_LINK_SOURCE  YES)
set(DOXYGEN_SOURCE_TOOLTIPS         YES)
set(DOXYGEN_USE_HTAGS               NO)
set(DOXYGEN_VERBATIM_HEADERS        YES)

# --- HTML output -------------------------------------------------------------
set(DOXYGEN_GENERATE_HTML           YES)
set(DOXYGEN_HTML_OUTPUT             html)
set(DOXYGEN_HTML_FILE_EXTENSION     .html)
set(DOXYGEN_HTML_COLORSTYLE         AUTO_LIGHT)
set(DOXYGEN_HTML_DYNAMIC_SECTIONS   YES)
set(DOXYGEN_GENERATE_TREEVIEW       YES)
set(DOXYGEN_DISABLE_INDEX           NO)
set(DOXYGEN_FULL_SIDEBAR            NO)
set(DOXYGEN_ENUM_VALUES_PER_LINE    4)
set(DOXYGEN_SEARCHENGINE            YES)

# --- Other formats — off by default for a portfolio/demo repo ----------------
set(DOXYGEN_GENERATE_LATEX          NO)
set(DOXYGEN_GENERATE_RTF            NO)
set(DOXYGEN_GENERATE_MAN            NO)
set(DOXYGEN_GENERATE_XML            NO)

# --- Preprocessor ------------------------------------------------------------
set(DOXYGEN_ENABLE_PREPROCESSING    YES)
set(DOXYGEN_MACRO_EXPANSION         NO)
set(DOXYGEN_EXPAND_ONLY_PREDEF      NO)
set(DOXYGEN_SEARCH_INCLUDES         YES)

# --- Graphviz / dot — component, dependency, collaboration diagrams ----------
set(DOXYGEN_HAVE_DOT                YES)
set(DOXYGEN_DOT_NUM_THREADS         0)
set(DOXYGEN_DOT_IMAGE_FORMAT        svg)
set(DOXYGEN_INTERACTIVE_SVG         YES)
set(DOXYGEN_DOT_TRANSPARENT         YES)
set(DOXYGEN_DOT_GRAPH_MAX_NODES     100)
set(DOXYGEN_MAX_DOT_GRAPH_DEPTH     0)
set(DOXYGEN_DOT_MULTI_TARGETS       YES)

set(DOXYGEN_UML_LOOK                YES)
set(DOXYGEN_UML_LIMIT_NUM_FIELDS    10)
set(DOXYGEN_TEMPLATE_RELATIONS      YES)

# Architecture-level graphs (the "component diagram" substitutes)
set(DOXYGEN_DIRECTORY_GRAPH         YES)
set(DOXYGEN_DIR_GRAPH_MAX_DEPTH     5)
set(DOXYGEN_GROUP_GRAPHS            YES)
set(DOXYGEN_COLLABORATION_GRAPH     YES)
set(DOXYGEN_INCLUDE_GRAPH           YES)
set(DOXYGEN_INCLUDED_BY_GRAPH       YES)

# Implementation-level graphs — off to keep diagrams architectural, not noisy
set(DOXYGEN_CLASS_GRAPH             NO)
set(DOXYGEN_CALL_GRAPH              NO)
set(DOXYGEN_CALLER_GRAPH            NO)
set(DOXYGEN_GRAPHICAL_HIERARCHY     YES)

set(DOXYGEN_DOT_CLEANUP             YES)

# PlantUML — for hand-authored component diagrams alongside generated ones.
if(PLANTUML_JARFILE)
  set(DOXYGEN_PLANTUML_JAR_PATH     "${PLANTUML_JARFILE}")
endif()

# doxygen_add_docs() treats its positional arguments as the INPUT list; the
# starter Doxyfile uses "include src README.md".
doxygen_add_docs(docs
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/README.md"
  COMMENT "Generating API documentation with Doxygen"
)
