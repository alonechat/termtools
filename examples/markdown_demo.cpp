#include "termtools.h"
#include <iostream>

using namespace termtools;

int main() {
    Console console;
    console.println();
    
    Text title("Markdown Rendering Demo");
    title.styled(StyleBuilder().bold().color(Color(ColorName::Cyan)));
    title.align(Justify::Center, console.getTerminalWidth());
    console.println(title);
    console.println();
    console.flush();

    std::string markdown = R"(# Heading 1

This is a paragraph with **bold text**, *italic text*, and ***bold italic text***.

## Heading 2

Here's some `inline code` and a [link example](https://example.com).

### Heading 3

You can also use ~~strikethrough~~ text.

#### Heading 4

- Unordered list item 1
- Unordered list item 2
- Unordered list item 3

##### Heading 5

1. Ordered list item 1
2. Ordered list item 2
3. Ordered list item 3

###### Heading 6

> This is a blockquote.
> It can span multiple lines.

---

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, Markdown!" << std::endl;
    return 0;
}
```

That's all the markdown features!
)";

    console.println("=== Default Theme ===");
    console.println();
    console.flush();
    Markdown::print(markdown);
    std::cout << std::flush;

    console.println();
    console.println("=== Dark Theme ===");
    console.println();
    console.flush();
    Markdown::print(markdown, MarkdownConfig::darkTheme());
    std::cout << std::flush;

    console.println();
    console.println("=== Colorful Theme ===");
    console.println();
    console.flush();
    Markdown::print(markdown, MarkdownConfig::colorfulTheme());
    std::cout << std::flush;

    console.println();
    console.println("=== Custom Config ===");
    console.println();
    console.flush();
    
    MarkdownConfig customConfig;
    customConfig.headingColor(Color(255, 100, 50))
                .codeColor(Color(100, 255, 100))
                .linkColor(Color(100, 200, 255))
                .quoteColor(Color(200, 200, 100))
                .listBulletColor(Color(255, 100, 200))
                .bulletPoint("►")
                .quotePrefix("▎ ");
    
    Markdown md(markdown, customConfig);
    md.print();
    std::cout << std::flush;

    return 0;
}
