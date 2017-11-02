context("use non-latin text (requires poppler-data)")

test_that("reading Chinese text", {
  suppressMessages(text <- pdf_text("chinese.pdf"))
  expect_length(text, 1)
  expect_equal(Encoding(text), "UTF-8")
  expect_true(grepl("\u98A8\u96AA\u56E0\u7D20", text))
})
