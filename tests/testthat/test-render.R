context("rendering pdf files")

test_that("rendering pdf", {
  # Expected output files
  outfiles <- c("pdf-example-password.original_1.png", "pdf-example-password.original_2.png",
    "pdf-example-password.original_3.png", "pdf-example-password.original_4.png")

  # Read with password
  expect_equal(pdf_convert("pdf-example-password.original.pdf", upw = "test"), outfiles)
  expect_true(all(file.exists(outfiles)))
  unlink(outfiles)
})
