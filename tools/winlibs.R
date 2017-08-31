# Build against mingw-w64 build of poppler 0.57.0
VERSION <- commandArgs(TRUE)
if(!file.exists(sprintf("../windows/poppler-%s/include/poppler/cpp/poppler-document.h", VERSION))){
  if(getRversion() < "3.3.0") setInternet2()
  download.file(sprintf("https://github.com/rwinlib/poppler/archive/v%s.zip", VERSION),
                "lib.zip", quiet = TRUE)
  dir.create("../windows", showWarnings = FALSE)
  unzip("lib.zip", exdir = "../windows")
  unlink("lib.zip")
}
