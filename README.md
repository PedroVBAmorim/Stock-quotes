# Stock Quote Fetcher

Fetches real-time stock quotes from the [Alpha Vantage API](https://www.alphavantage.co/), prints a summary, and saves the full JSON response to a file.

## Features

- Fetch stock quotes over HTTP using **libcurl**.
- Parse JSON using **nlohmann/json**.
- Display price, change, and percentage change in the console.
- Save the full JSON response to `<SYMBOL>_quote.json`.

## Libraries Used

- **libcurl** – for HTTP requests.
- **nlohmann/json** – for JSON parsing.
- Standard C++ libraries: `iostream`, `string`, `algorithm`, `fstream`.

## Requirements

- C++ compiler (supports at least C++11 for `auto` and lambda support).
- [libcurl](https://curl.se/libcurl/) installed.
- `nlohmann/json.hpp` included in the project.
- Alpha Vantage API key.

## Setup

1. Place your API key in `main()`:

cpp
