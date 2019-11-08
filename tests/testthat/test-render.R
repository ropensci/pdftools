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
