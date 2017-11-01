.onLoad <- function(lib, pkg){
  set_error_callback()
  datapath <- normalizePath(file.path(lib, pkg, "share", "poppler"), mustWork = FALSE)
  if(file.exists(datapath))
    set_poppler_data(datapath)
}

err_cb <- function(str){
  warning(str, call. = FALSE, immediate. = TRUE)
}
