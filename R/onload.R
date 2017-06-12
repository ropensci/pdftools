.onLoad <- function(lib, pkg){
  set_error_callback()
}

err_cb <- function(str){
  warning(str, call. = FALSE, immediate. = TRUE)
}
