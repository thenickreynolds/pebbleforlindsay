#include <pebble.h>

static Window *window;
static Layer *window_layer;
static GRect bounds;
static struct PageItem *home;
static struct PageItem *current;

struct TextItem {
  TextLayer *layer;
  struct TextItem *next;
};

struct PageItem {
  struct PageItem *next;
  struct PageItem *previous;
  struct TextItem *items;
};

static TextLayer *create_text_layer(char *text, int top) {
  TextLayer *layer = text_layer_create(GRect(0, top, bounds.size.w, 20));
  text_layer_set_text(layer, text);
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
  return layer;
}

static void add_text_to_page(struct PageItem *page, char *text, int top) {
  TextLayer *layer = create_text_layer(text, top);
  struct TextItem *textItem = (struct TextItem *)malloc(sizeof(struct TextItem));
  textItem->layer = layer;
  textItem->next = NULL;
  
  if (page->items == NULL) {
    // insert at head
    page->items = textItem;
  } else {
    // find tail and insert at tail
    struct TextItem *tail = page->items;
    while (tail->next != NULL) {
      tail = tail->next;
    }
    
    tail->next = textItem;
  }
}

static struct PageItem *create_page() {  
  struct PageItem *page = (struct PageItem *)malloc(sizeof(struct PageItem));
  page->next = NULL;
  page->items = NULL;

  if (home == NULL) {
    home = page;
    page->previous = NULL;
  } else {
    struct PageItem *tail = home;
    while (tail->next != NULL) {
      tail = tail->next;
    }
    tail->next = page;
    page->previous = tail;
  }
  
  return page;
}

static void create_page_with_content(char *line1, char *line2, char *line3, char *line4, char *line5, char *line6) {
  int start = 35;
  int interval = 20;

  struct PageItem *page = create_page();
  
  char *lines[6] = { line1, line2, line3, line4, line5, line6 };
  
  for (int i = 0; i < 6; i++) {
    int offset = start + (interval * i);
    if (lines[i] != NULL) {
      add_text_to_page(page, lines[i], offset);
    }
  }
}

static void create_pages() {
  create_page_with_content(
    NULL,
    "Happy Birthday",
    "Lindsay!",
    NULL,
    "(press down)",
    NULL);

  create_page_with_content(
    "- a haiku -",
    NULL,
    "My beautiful girl,",
    "so intelligent and fun;",
    "I do love you so!",
    NULL);

  create_page_with_content(
    "- a ballad -",
    NULL,
    "Lindsay, Lindz you're so fine,",
    "like a nice red wine",
    "I'm so glad you're mine!",
    NULL);
  
  create_page_with_content(
    "- a definition -",
    NULL,
    "Lindsay (n), [lin-see]",
    "1. talented, smart, hilarious",
    "2. beautiful, caring, loving",
    "3. my love");
  
  create_page_with_content(
    "- an alt. definition -",
    NULL,
    "Slindsay (n), [slin-see]",
    "1. life of the party",
    "2. a hilarious mess",
    "3. trouble");
  
  create_page_with_content(
    "- an offer -",
    "[VALID UNTIL 7/15/2017]",
    "One custom Pebble Watch",
    "application for whatever",
    "the bearer chooses",
    "@>'-,--");
  
  create_page_with_content(
    "- travel -",
    NULL,
    "An all expenses paid trip to",
    "Portland for a romantic",
    "(and party?)",
    "weekend away!");
  
  create_page_with_content(
    "- with all my love -",
    NULL,
    "for your 25th year",
    "Happy Birthday again!",
    "<3 your nerd",
    "Nick xoxo");
}

static void show_page() {
  layer_remove_child_layers(window_layer);
  struct TextItem *text = current->items;
  while (text != NULL) {
    layer_add_child(window_layer, text_layer_get_layer(text->layer));
    text = text->next;
  }
}

static void show_pages() {
  current = home;
  show_page();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // TODO
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (current->previous != NULL) {
    current = current->previous;
    show_page();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (current->next != NULL) {
    current = current->next;
    show_page();
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);
  
  create_pages();
  show_pages();
}

static void window_unload(Window *window) {
  struct PageItem *page = home;
  while (page != NULL) {
    struct TextItem *text = page->items;
    while (text != NULL) {
      text_layer_destroy(text->layer);
      struct TextItem *textToFree = text;
      text = text->next;
      free(textToFree);
    }
    struct PageItem *pageToFree = page;
    page = page->next;
    free(pageToFree);
  }
  home = NULL;
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}