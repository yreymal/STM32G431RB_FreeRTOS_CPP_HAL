if(ENABLE_COLOR)
    string(ASCII 27 ESC)
    set(RESET "${ESC}[0m")
    set(BOLD "${ESC}[1m")
    set(CYAN "${ESC}[36m")
    set(GREEN "${ESC}[32m")
    set(YELLOW "${ESC}[33m")
    set(MAGENTA "${ESC}[35m")
else()
    set(RESET "")
    set(BOLD "")
    set(CYAN "")
    set(GREEN "")
    set(YELLOW "")
    set(MAGENTA "")
endif()

execute_process(
    COMMAND "${SIZE_TOOL}" "${ELF_FILE}"
    RESULT_VARIABLE SIZE_RESULT
    OUTPUT_VARIABLE SIZE_OUTPUT
    ERROR_VARIABLE SIZE_ERROR
)

if(NOT SIZE_RESULT EQUAL 0)
    message(FATAL_ERROR "Unable to read firmware size: ${SIZE_ERROR}")
endif()

string(REGEX MATCHALL "[^ \t\r\n]+" SIZE_FIELDS "${SIZE_OUTPUT}")
list(GET SIZE_FIELDS 6 TEXT_BYTES)
list(GET SIZE_FIELDS 7 DATA_BYTES)
list(GET SIZE_FIELDS 8 BSS_BYTES)
list(GET SIZE_FIELDS 9 TOTAL_BYTES)

file(SIZE "${ELF_FILE}" ELF_BYTES)
file(SIZE "${HEX_FILE}" HEX_BYTES)
file(SIZE "${BIN_FILE}" BIN_BYTES)

message("")
message("${BOLD}${GREEN}Build completed successfully${RESET}")
message("${BOLD}${CYAN}Firmware section sizes${RESET}")
message("  text: ${YELLOW}${TEXT_BYTES}${RESET} B"
        "  data: ${YELLOW}${DATA_BYTES}${RESET} B"
        "  bss: ${YELLOW}${BSS_BYTES}${RESET} B"
        "  total: ${YELLOW}${TOTAL_BYTES}${RESET} B")
message("${BOLD}${MAGENTA}Generated firmware artifacts${RESET}")
message("  ${GREEN}ELF${RESET}: ${ELF_FILE} (${ELF_BYTES} B)")
message("  ${GREEN}HEX${RESET}: ${HEX_FILE} (${HEX_BYTES} B)")
message("  ${GREEN}BIN${RESET}: ${BIN_FILE} (${BIN_BYTES} B)")
message("")
