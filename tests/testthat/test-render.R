context("rendering pdf files")

test_that("rendering pdf", {
  # Expected output files
  outfiles <- c("pdf-example-password.original_1.png", "pdf-example-password.original_2.png",
    "pdf-example-password.original_3.png", "pdf-example-password.original_4.png")

  # Read with password
  expect_equal(pdf_convert("pdf-example-password.original.pdf", upw = "test", verbose = FALSE), outfiles)
  expect_true(all(file.exists(outfiles)))
  unlink(outfiles)

  # Use filenames format string
  outfiles <- paste0("test_0", 1:4, ".png")
  expect_equal(
    pdf_convert("pdf-example-password.original.pdf", upw = "test", verbose = FALSE, filenames = "test_%02d.%s"),
    outfiles
  )
  expect_true(all(file.exists(outfiles)))
  unlink(outfiles)
})

test_that("rendering with different bg colors", {
  # PDF with a fully transparent background: opaque bg args should be applied verbatim
  f1 <- tempfile(fileext = ".pdf")
  on.exit(unlink(f1), add = TRUE)
  grDevices::pdf(f1, bg = "transparent")
  grid::grid.newpage()
  invisible(grDevices::dev.off())

  r_white <- as.raster(pdf_render_page(f1, bg = "white", numeric = TRUE))
  expect_equal(as.character(r_white[1, 1]), "#FFFFFFFF")

  r_black <- as.raster(pdf_render_page(f1, bg = "black", numeric = TRUE))
  expect_equal(as.character(r_black[1, 1]), "#000000FF")

  # PDF with a semi-transparent background
  f2 <- tempfile(fileext = ".pdf")
  on.exit(unlink(f2), add = TRUE)
  grDevices::pdf(f2, bg = "#0000FF80")
  grid::grid.newpage()
  invisible(grDevices::dev.off())

  # Rendered on a transparent background the original color should be
  # preserved exactly since there is nothing to composite against (#141)
  skip_if_not(poppler_config()$can_render_transparent_bg)
  r_transparent <- as.raster(pdf_render_page(f2, bg = "transparent", numeric = TRUE))
  expect_equal(as.character(r_transparent[1, 1]), "#0000FF80")

  # Semi-transparent backgrounds are not supported
  expect_error(pdf_render_page(f2, bg = "#0000FF80"), "opaque")
})
