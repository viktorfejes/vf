# Clear the screen
# Clear-Host

# Define ANSI escape sequences
$esc = [char]27
$red = "${esc}[38;2;225;0;0m"
$green = "${esc}[38;2;0;200;0m"
$yellow = "${esc}[38;2;200;200;0m"
$blue = "${esc}[38;2;0;128;200m"
$reset = "${esc}[0m"

# Output text with ANSI escape sequences for background color and text formatting
Write-Host "[VF TESTS] Running 7 tests..."
Write-Host ""
Write-Host "${red}${esc}[7m FAIL ${reset} DynamicArray/Create .......................... (0.089s)"
Write-Host "  ${green}✔  EXPECT_EQ_INT: Expected values are equal at ./test_vf_darray.h:19${reset}"
Write-Host "  ${red}❌ EXPECT_EQ_INT: Expected values to be equal at ./test_vf_darray.h:19${reset}"
Write-Host "    Expected: ${green}128${reset}"
Write-Host "    Received: ${esc}[38;2;200;0;0m512${reset}"
Write-Host ""
Write-Host "  ${green}✔  EXPECT_EQ_INT: Expected values are equal at ./test_vf_darray.h:19${reset}"
Write-Host ""
Write-Host "${esc}[38;2;0;180;0m${esc}[7m PASS ${reset} DynamicArray/Push ............................ (0.091s)"
Write-Host "  ${green}✔  EXPECT_EQ_INT: Expected values are equal at ./test_vf_darray.h:19${reset}"
Write-Host "  ${green}✔  EXPECT_EQ_FLOAT: Expected values are equal at ./test_vf_darray.h:19${reset}"
Write-Host "  ${green}✔  EXPECT_TRUE: Expected condition is true ./test_vf_darray.h:19${reset}"

Write-Host ""
Write-Host "${yellow}${esc}[7m SKIP ${reset} DynamicArray/Destroy ......................... (0.123s)"
Write-Host ""
Write-Host "Test Suites: ${red}1 failed${reset}, 1 total"
Write-Host "Tests:       ${red}1 failed${reset}, 2 passed, 3 total"
Write-Host "Time:        1.234s"