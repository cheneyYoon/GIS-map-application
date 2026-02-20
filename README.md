# AtlasVision: GoogleMaps for drivers

AtlasVision is an advanced Geographic Information System (GIS) application designed for interactive map visualization, spatial analysis, and algorithmic exploration. Built with C++ and leveraging custom graphics and UI libraries, AtlasVision provides a robust platform for both educational and practical applications in urban planning, navigation, and geospatial data analysis.

## Features
- **Interactive Map Rendering:** Visualize and interact with detailed maps using a custom graphics engine.
- **Algorithmic Exploration:** Includes modules for pathfinding, distance calculations, and spatial queries.
- **Modular Architecture:** Organized into clear modules for algorithms, UI, drawing, and data loading.
- **Performance Testing:** Integrated performance and unit tests for algorithm validation and benchmarking.
- **Custom UI:** User interface designed with Qt (main.ui) for intuitive map interaction.

## Project Structure
```
libstreetmap/
  src/           # Core logic: algorithms, helpers, drawing, loading, UI helpers
  ezgl/          # Custom graphics and UI library
  resources/     # UI resources (Qt .ui files)
  tests/         # Performance and unit tests
main/
  src/           # Application entry point
Makefile         # Build instructions
```

## Getting Started
1. **Clone the repository:**
   ```sh
   git clone https://github.com/cheneyYoon/GIS-map-application.git
   cd GIS-map-application
   ```
2. **Build the project:**
   ```sh
   make
   ```
3. **Run the application:**
   ```sh
   ./main/src/main
   ```

## Dependencies
- C++17 or later
- Qt (for UI)
- Standard C++ libraries

## Usage
- Launch the application to explore map data interactively.
- Use built-in tools for spatial analysis and algorithm visualization.
- Run tests in the `libstreetmap/tests/` directory for validation and benchmarking.

## Contributing
Contributions are welcome! Please open issues or submit pull requests for improvements, bug fixes, or new features.

## License
This project is licensed under the MIT License.

