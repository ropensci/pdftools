context("page sizes")

# rotated-boxes.pdf is a tiny hand-written (plain text) PDF with four blank US
# letter pages, chosen because R's pdf() device only writes a MediaBox.
# /MediaBox [0 0 612 792] and /Rotate 90 are set on the Pages node, so pages
# inherit them unless they override them:
#   page 1: /CropBox [36 36 576 756] /TrimBox [72 72 540 720]  (inherits /Rotate 90)
#   page 2: /Rotate 0
#   page 3: /Rotate -90 /MediaBox [0 0 595 842]                 (-90 normalizes to 270)
#   page 4: /Rotate 180
# The file uses CRLF line endings throughout so that git's autocrlf (the
# default on Windows) leaves it alone: xref offsets are byte-exact and would
# otherwise be corrupted by LF -> CRLF conversion on checkout.
boxes_pdf <- "rotated-boxes.pdf"

test_that("pdf_pagesize() reports page rotation", {
  # Boxes are in the unrotated coordinate system; display_* swap width and
  # height for rotations of 90 and 270 only
  expect_equal(pdf_pagesize(boxes_pdf), data.frame(
    top = c(36, 0, 0, 0),
    right = c(576, 612, 595, 612),
    bottom = c(756, 792, 842, 792),
    left = c(36, 0, 0, 0),
    width = c(540, 612, 595, 612),
    height = c(720, 792, 842, 792),
    rotation = c(90L, 0L, 270L, 180L),
    display_width = c(720, 612, 842, 612),
    display_height = c(540, 792, 595, 792)
  ))
})

test_that("display size matches rendered output", {
  skip_if_not(poppler_config()$can_render)
  df <- pdf_pagesize(boxes_pdf)
  for (i in seq_len(nrow(df))) {
    dims <- dim(pdf_render_page(boxes_pdf, page = i, dpi = 72))
    expect_equal(dims[2:3], c(df$display_width[i], df$display_height[i]))
  }
})

test_that("pdf_pagesize() box argument", {
  crop <- pdf_pagesize(boxes_pdf)

  # Only page 1 sets a CropBox and TrimBox, so only its row differs from crop
  media <- pdf_pagesize(boxes_pdf, box = "media")
  expect_equal(media[-1, ], crop[-1, ])
  expect_equal(unlist(media[1, c("left", "width", "height", "display_width")]),
    c(left = 0, width = 612, height = 792, display_width = 792))

  # Pages without a TrimBox fall back to the CropBox
  trim <- pdf_pagesize(boxes_pdf, box = "trim")
  expect_equal(trim[-1, ], crop[-1, ])
  expect_equal(unlist(trim[1, c("left", "width", "height", "display_width")]),
    c(left = 72, width = 468, height = 648, display_width = 648))

  # No page defines a BleedBox or ArtBox, so these equal the CropBox
  expect_equal(pdf_pagesize(boxes_pdf, box = "bleed"), crop)
  expect_equal(pdf_pagesize(boxes_pdf, box = "art"), crop)

  expect_error(pdf_pagesize(boxes_pdf, box = "foo"))
})

test_that("pdf_pagesize() sees pages rotated by qpdf", {
  # hello.pdf is a single unrotated US letter page
  r <- tempfile(fileext = ".pdf")
  on.exit(unlink(r), add = TRUE)
  qpdf::pdf_rotate_pages("hello.pdf", pages = 1L, angle = 90, output = r)
  orig <- pdf_pagesize("hello.pdf")
  rotated <- pdf_pagesize(r)
  expect_identical(orig$rotation, 0L)
  expect_identical(rotated$rotation, 90L)
  # qpdf only sets /Rotate, so the stored box is unchanged but the displayed
  # size has width and height swapped
  expect_equal(rotated[c("width", "height")], orig[c("width", "height")])
  expect_equal(rotated$display_width, orig$height)
  expect_equal(rotated$display_height, orig$width)
})
