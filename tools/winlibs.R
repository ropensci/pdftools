# Build against mingw-w64 build of poppler 0.42.0
if(!file.exists("../windows/poppler-0.42.0/include/poppler/cpp/poppler-document.h")){
  if(getRversion() < "3.3.0") setInternet2()
  download.file("https://github.com/rwinlib/poppler/archive/v0.42.0.zip", "lib.zip", quiet = TRUE)
  dir.create("../windows", showWarnings = FALSE)
  unzip("lib.zip", exdir = "../windows")
  unlink("lib.zip")
}
