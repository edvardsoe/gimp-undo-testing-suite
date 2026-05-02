/* GIMP - The GNU Image Manipulation Program
 *
 * Basic undo/redo validation tests.
 */

#include "config.h"

#include <gegl.h>

#include "core/core-types.h"
#include "core/gimpimage-undo.h"

#include "tests.h"


static void
test_undo_with_null_image (void)
{
  gboolean result;

  g_test_expect_message (NULL,
                         G_LOG_LEVEL_CRITICAL,
                         "*assertion*GIMP_IS_IMAGE*failed*");

  result = gimp_image_undo (NULL);

  g_test_assert_expected_messages ();

  g_assert_false (result);
}


static void
test_redo_with_null_image (void)
{
  gboolean result;

  g_test_expect_message (NULL,
                         G_LOG_LEVEL_CRITICAL,
                         "*assertion*GIMP_IS_IMAGE*failed*");

  result = gimp_image_redo (NULL);

  g_test_assert_expected_messages ();

  g_assert_false (result);
}


int
main (int    argc,
      char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/gimp-undo/undo-null-image",
                   test_undo_with_null_image);

  g_test_add_func ("/gimp-undo/redo-null-image",
                   test_redo_with_null_image);

  return g_test_run ();
}