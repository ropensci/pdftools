context("reading pdf files")

test_that("reading password protected pdf", {
  # Read with password
  if(poppler_config()$version >= "0.30"){
    expect_message(expect_true(pdf_info("pdf-example-password.original.pdf")$locked))
    expect_error(expect_message(pdf_text("pdf-example-password.original.pdf"), "locked"))
  } else{
    expect_true(pdf_info("pdf-example-password.original.pdf")$locked)
    expect_error(pdf_text("pdf-example-password.original.pdf"), "locked")
  }

  # Get text with password
  expect_equal(4, length(pdf_text("pdf-example-password.original.pdf", upw = "test")))
  expect_false(pdf_info("pdf-example-password.original.pdf", upw = "test")$locked)

  # Reading 'encrypted' file
  info <- pdf_info("pdf-example-encryption.original.pdf")
  expect_true(info$encrypted)
  expect_false(info$locked)
})
