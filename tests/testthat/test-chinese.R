context("use non-latin text (requires poppler-data)")

test_that("reading Chinese text", {
  suppressMessages(text <- pdf_text("chinese.pdf"))
  expect_length(text, 1)
  expect_equal(Encoding(text), "UTF-8")
  expect_match(text, "\u98A8\u96AA\u56E0\u7D20")

  # Check fonts
  fonts <- pdf_fonts("chinese.pdf")
  expect_equal(nrow(fonts), 3)
  expect_true(all(fonts$embedded))
  expect_match(fonts$name[2], "MSung")
  expect_match(fonts$name[3], "MHei")
})
