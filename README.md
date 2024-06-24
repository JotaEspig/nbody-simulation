# nbody-simulation
N-Body simulation using Axolote Engine

## Dependencies
* CMake
* OpenGL
* GLM
* GLFW3
* Doxygen (Optional)
* Axolote Engine. See [https://github.com/JotaEspig/axolote-engine](https://github.com/JotaEspig/axolote-engine) 

## Compiling
```bash
git clone https://github.com/JotaEspig/nbody-simulation.git
cd nbody-simulation
cmake .
make -j4
```
You need to install [Axolote Engine](https://github.com/JotaEspig/axolote-engine) to run it.
```bash
chmod +x install_axolote.sh
./install_axolote.sh
```

## Running
After compiling
```bash
./bin/nbody-simulation <config json file>
```

There are some config files already made inside `config` directory. But you can create your custom config file using:
```bash
python3 scripts/config_generator.py
```

### Keybinds

* `W`, `A`, `S`, `D`, `LEFT_SHIFT`, `SPACE` to move the camera around the focus point (default is (0, 0, 0))
* `UP`, `DOWN`, `LEFT`, `RIGHT`, `RIGHT_SHIFT`, `RIGHT_CONTROL` to move the focus point
* `R` to reset the camera position and set the focus point to be (0, 0, 0)
* `P` to pause the simulation
* `X` to throw a "semi" massive body where the camera is pointing (WHEN DOING REAL TIME SIMULATION)
* `ESC` to quit/close the window

## Documentation
You can generate the documentation using Doxygen
```bash
make docs
```
Then open `docs/html/index.html` with your browser.

## Class Diagram (may be outdated)
[Diagram](docs/diagram.png)
```mermaid
classDiagram
    class App {
        +bool pause
        +std::shared_ptr~CelestialBodySystem~ bodies_system
        +void bake(const char* json_filename)
        +void main_loop(const char* json_filename)
        +void process_input(float delta_t)
        +void process_input_real_time_mode(float delta_t)
        +void render_loop(const char* json_filename)
    }

    class CelestialBody {
        +CelestialBody(double mass, const glm::vec3& velocity, const glm::vec3& pos)
        +bool is_colliding(const CelestialBody& other)
        +bool merged
        #double _mass
        +double mass()
        #float _radius
        +float radius()
        +glm::mat4 mat
        #glm::vec3 _color
        +glm::vec3 calculate_acceleration_vec(const CelestialBody& other)
        +glm::vec3 calculate_acceleration_vec(const glm::vec3& pos, double mass)
        +glm::vec3 color()
        +glm::vec3 pos
        +glm::vec3 velocity
        +void merge(std::shared_ptr~CelestialBody~ other)
        +void set_mass(double mass)
        +void update_matrix()
        +void update_values()
    }

    class CelestialBodySystem {
        +CelestialBodySystem()
        +~CelestialBodySystem()
        +OcTree octree
        +Sphere sphere
        +axolote::gl::Shader get_shader()
        -axolote::gl::VBO instanced_colors_vbo
        -axolote::gl::VBO instanced_matrices_vbo
        +std::shared_ptr~CelestialBody~ add_body(double mass, glm::vec3 pos, glm::vec3 vel)
        -std::vector~std::shared_ptr~CelestialBody~~ _celestial_bodies
        +std::vector~std::shared_ptr~CelestialBody~~ celestial_bodies()
        +void barnes_hut_algorithm(double dt)
        +void bind_shader(const axolote::gl::Shader& shader_program)
        -void build_octree()
        +void draw()
        +void draw(const glm::mat4& mat)
        +void naive_algorithm(double dt)
        +void setup_instanced_vbo()
        +void setup_using_baked_frame_json(nlohmann::json& data)
        +void setup_using_json(nlohmann::json& data)
        +void update(double dt)
        +void update_vbos()
    }

    class OcTree {
        +OcTree()
        +OcTree(float initial_coord)
        +float initial_coord
        +float initial_width
        +glm::vec3 net_acceleration_on_body(std::shared_ptr~CelestialBody~ body, double dt)
        +static double theta
        +std::unique_ptr~Node~ root
        +void insert(const std::shared_ptr~CelestialBody~& body)
    }

    class OcTree_Node {
        +Node()
        +Node(glm::vec3 cube_start, float width)
        +bool is_leaf
        -bool should_be_called(const std::shared_ptr~CelestialBody~& other)
        +double ratio_width_distance(const glm::vec3& pos)
        +double total_mass
        +float width
        +glm::vec3 center_of_mass
        +glm::vec3 cube_start
        +glm::vec3 net_acceleration_on_body(std::shared_ptr~CelestialBody~ body, double dt)
        +std::ostream& operator<<(std::ostream& os, Node node)
        +std::ostream& operator<<(std::ostream& os, std::unique_ptr~Node~& node)
        +std::shared_ptr~CelestialBody~ body
        +std::unique_ptr~Node~ lbb
        +std::unique_ptr~Node~ lbf
        +std::unique_ptr~Node~ lub
        +std::unique_ptr~Node~ luf
        +std::unique_ptr~Node~ rbb
        +std::unique_ptr~Node~ rbf
        +std::unique_ptr~Node~ rub
        +std::unique_ptr~Node~ ruf
        +std::unique_ptr~Node~& find_correct_child(const glm::vec3& pos)
        +void insert(const std::shared_ptr~CelestialBody~& body)
        +void split()
    }

    class Sphere {
        +Sphere()
        +axolote::gl::EBO indices_ebo
        +axolote::gl::Shader get_shader()
        -axolote::gl::Shader shader
        +axolote::gl::VAO vao
        +axolote::gl::VBO vertices_vbo
        -std::vector~GLuint~ _indices
        +std::vector~GLuint~ indices()
        -std::vector~glm::vec3~ _vertices
        +void bind_shader(const axolote::gl::Shader& shader_program)
        +void draw()
        +void draw(const glm::mat4& mat)
        +void update(double dt)
    }

    App *-- CelestialBodySystem
    CelestialBodySystem *-- CelestialBody
    CelestialBodySystem *-- OcTree
    CelestialBodySystem *-- Sphere
    OcTree_Node *-- CelestialBody
    OcTree *-- OcTree_Node 
```
