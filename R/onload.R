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

# Load tibble (if available) for pretty printing
.onAttach <- function(lib, pkg){
  packageStartupMessage(sprintf('Using poppler version %s', poppler_config()$version))
  if(interactive() && is.null(.getNamespace('tibble'))){
    tryCatch({
      getNamespace('tibble')
    }, error= function(e){})
  }
}
