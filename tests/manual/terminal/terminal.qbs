import qbs

QtcAutotest {
    name: "Terminal autotest"
    Depends { name: "Utils" }
    files: "tst_terminal.cpp"
}
