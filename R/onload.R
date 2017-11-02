.onLoad <- function(lib, pkg){
  set_error_callback()
  datapath <- normalizePath(file.path(lib, pkg, "share", "poppler"), mustWork = FALSE)
  if(file.exists(datapath))
    set_poppler_data(datapath)
}

# See https://twitter.com/jimhester_/status/918853208877002757
err_cb <- function(str){
  message(conditionMessage(simpleMessage(paste("PDF", str))))
}
