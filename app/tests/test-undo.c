

//REDO/UNDO TESTS

#include "config.h"
#include <gegl.h>
#include <gtk/gtk.h>
#include "widgets/widgets-types.h"
#include "widgets/gimpuimanager.h"
#include "core/gimp.h"
#include "core/gimpimage.h"
#include "core/gimpimage-undo.h"
#include "core/gimplayer.h"
#include "core/gimplayer-new.h"
#include "tests.h"
#include "gimp-app-test-utils.h"


#define GIMP_TEST_IMAGE_SIZE 100


typedef struct
{
  GimpImage *image;
} GimpUndoTestFixture;


static void
gimp_undo_test_image_setup (GimpUndoTestFixture *fixture,
                            gconstpointer        data)
{
  Gimp *gimp = GIMP (data);

  fixture->image = gimp_image_new (gimp,
                                   GIMP_TEST_IMAGE_SIZE,
                                   GIMP_TEST_IMAGE_SIZE,
                                   GIMP_RGB,
                                   GIMP_PRECISION_FLOAT_LINEAR);
}


static void
gimp_undo_test_image_teardown (GimpUndoTestFixture *fixture,
                               gconstpointer        data)
{
  g_object_unref (fixture->image);
}


static void
add_test_layer (GimpImage   *image,
                const gchar *name,
                gboolean     push_undo)
{
  GimpLayer *layer;
  gboolean   result;

  layer = gimp_layer_new (image,
                          GIMP_TEST_IMAGE_SIZE,
                          GIMP_TEST_IMAGE_SIZE,
                          babl_format ("R'G'B'A u8"),
                          name,
                          GIMP_OPACITY_OPAQUE,
                          GIMP_LAYER_MODE_NORMAL);

  g_assert_true (GIMP_IS_LAYER (layer));

  result = gimp_image_add_layer (image,
                                 layer,
                                 NULL,
                                 0,
                                 push_undo);

  g_assert_true (result);
}


static void
test_add_layer_undo_redo (GimpUndoTestFixture *fixture,
                          gconstpointer        data)
{
  GimpImage *image = fixture->image;

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 0);

  add_test_layer (image, "Undo Test Layer", TRUE);

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 1);

  gimp_image_undo (image);

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 0);

  gimp_image_redo (image);

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 1);
}


static void
test_multiple_layer_undo_redo (GimpUndoTestFixture *fixture,
                               gconstpointer        data)
{
  GimpImage *image = fixture->image;

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 0);

  add_test_layer (image, "Layer 1", TRUE);
  add_test_layer (image, "Layer 2", TRUE);

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 2);

  gimp_image_undo (image);
  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 1);

  gimp_image_undo (image);
  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 0);

  gimp_image_redo (image);
  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 1);

  gimp_image_redo (image);
  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 2);
}


static void
test_redo_cleared_after_new_action (GimpUndoTestFixture *fixture,
                                    gconstpointer        data)
{
  GimpImage *image = fixture->image;

  add_test_layer (image, "Layer 1", TRUE);

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 1);

  gimp_image_undo (image);

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 0);

  add_test_layer (image, "Layer 2", TRUE);

  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 1);

  gimp_image_redo (image);

  //redo should do nothing because a new action cleared redo history
  g_assert_cmpint (gimp_image_get_n_layers (image), ==, 1);
}


#define ADD_UNDO_TEST(function)                                      \
  g_test_add ("/gimp-undo/" #function,                              \
              GimpUndoTestFixture,                                  \
              gimp,                                                 \
              gimp_undo_test_image_setup,                           \
              function,                                             \
              gimp_undo_test_image_teardown)


int
main (int    argc,
      char **argv)
{
  Gimp *gimp;
  int   result;

  g_test_init (&argc, &argv, NULL);

  gimp_test_utils_set_gimp3_directory ("GIMP_TESTING_ABS_TOP_SRCDIR",
                                       "app/tests/gimpdir");

  gimp_test_utils_set_gimp3_directory ("GIMP_TESTING_ABS_TOP_BUILDDIR",
                                       "app/tests/gimpdir-output");

  gimp = gimp_init_for_testing ();

  ADD_UNDO_TEST (test_add_layer_undo_redo);
  ADD_UNDO_TEST (test_multiple_layer_undo_redo);
  ADD_UNDO_TEST (test_redo_cleared_after_new_action);

  result = g_test_run ();

  gimp_exit (gimp, TRUE);

  return result;
}