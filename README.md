## GS1 barcode (EAN/UPC, Databar barcodes) parser library

This library parses the GS1 fields stream according to the [GS1 specification](https://www.gs1.org/docs/barcodes/GS1_General_Specifications.pdf).

### Example 1: simple listing
```cpp
#include <iostream>
#include <vector>
#include "gs1-parse.h"

void main()
{
    const char *gs1_stream = "010061414199999610ABCDEF123456" "\x1D" "21654321FEDCBA" "\x1D" "310200123411140823";

    std::vector<GS1::FieldAI> fields;
    parse_gs1(fields, gs1_stream);

    for(size_t i=0; i<fields.size(); ++i)
    {
        const GS1::FieldAI& f = fields[i];
        std::cout << "AI " << f.text_ai << " = " << f.text_body << "\r\n";
    }
}
```
Prints
```
AI 01 = 00614141999996
AI 10 = ABCDEF123456
AI 21 = 654321FEDCBA
AI 3102 = 001234
AI 11 = 140823
```

### Example 2: details
```cpp
#include <iostream>
#include <vector>
#include "gs1-parse.h"

void main()
{
    const char *gs1_stream = "010061414199999610ABCDEF123456" "\x1D" "21654321FEDCBA" "\x1D" "310200123411140823";

    std::vector<GS1::FieldAI> fields;
    parse_gs1(fields, gs1_stream);

    for(size_t i=0; i<fields.size(); ++i)
    {
        const GS1::FieldAI& f = fields[i];
        std::cout << i << ": AI " << f.text_ai << " (" << f.ai->data_title << ")\r\n"
            << "   " << f.ai->desc_content << "\r\n"
            << "   " << f.text_body << " - text\r\n"
            << "   " << f.format_body() << " - formatted\r\n";
    }
}
```
Prints
```
0: AI 01 (GTIN)
   Global Trade Item Number (GTIN)
   00614141999996 - text
   00614141999996 - formatted
1: AI 10 (BATCH/LOT)
   Batch or lot number
   ABCDEF123456 - text
   ABCDEF123456 - formatted
2: AI 21 (SERIAL)
   Serial number
   654321FEDCBA - text
   654321FEDCBA - formatted
3: AI 3102 (NET WEIGHT (kg))
   Net weight, kilograms
   001234 - text
   12.34 - formatted
4: AI 11 (PROD DATE)
   Production date (YYMMDD)
   140823 - text
   23-08-2014 - formatted
```

### Example 3: field search by AI group
```cpp
#include <iostream>
#include "gs1-parse.h"

void main()
{
    const char *gs1_stream = "010061414199999610ABCDEF123456" "\x1D" "21654321FEDCBA" "\x1D" "310200123411140823";

    GS1::FieldsGS1 gs1;
    gs1.ParseGS1(gs1_stream);

    //get product identity
    const GS1::FieldAI *gtin = gs1.GetByAI("01");
    if (gtin) std::cout << "GTIN = " << gtin->text_body << "\r\n";
    else std::cout << "GTIN not found\r\n";

    //get product net weight
    const GS1::FieldAI *weight = gs1.GetByAI("310n");
    if (weight)
    {
        std::cout << "WEIGHT: "
            << "AI group = " << weight->ai->ai << ", "
            << "AI code = " << weight->text_ai << ", "
            << "weight = " << weight->format_body() << " kg\r\n";
    }
    else std::cout << "WEIGHT not found\r\n";
}
```
Prints
```
GTIN = 00614141999996
WEIGHT: AI group = 310n, AI code = 3102, weight = 12.34 kg
```

### Additional links

- General specification (GS1 AI - Application Identifiers)
https://www.gs1.org/docs/barcodes/GS1_General_Specifications.pdf
- DataMatrix guideline
https://www.gs1.org/docs/barcodes/GS1_DataMatrix_Guideline.pdf
- Databar family
https://www.gs1.org/standards/barcodes/databar
https://www.gs1.org/docs/barcodes/databar/GS1_DataBar_Business_Case_Complete.pdf
https://www.gs1.org/docs/barcodes/GS1_Barcodes_Fact_Sheet-GS1_DataBar_family.pdf
