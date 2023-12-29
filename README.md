# NAT-Sim
#### Video Demo:  [<URL HERE>](https://youtu.be/jVh_pqTqDPs?si=yCMA0KaJSnCX62-e)https://youtu.be/jVh_pqTqDPs?si=yCMA0KaJSnCX62-e
#### Description:
Nat-Sim is a program that simulates a fantasy nation using various systems and algorithms. You can see how your nation changes over time based on different parameters.

Your nation starts with a settler unit that finds a good tile for a settlement. Settlements are where your citizens live and work. They can become cities by growing their population and borders. Cities can also send out new settlers to new tiles.

Your citizens have different attributes and statuses. They can work as craftsmen or gatherers. Craftsmen build infrastructure to improve the settlement. Gatherers collect food and materials to support the settlement.

Your population changes due to birth, death, migration, and marriage. Each citizen interacts with others to spread diseases and form families. Families can have children if they have enough resources. Population health checks are run per tick on each settlement. When a citizen dies, their cause of death and age are logged in the log box UI element and a text file.

Nat-Sim is a fantasy nation simulator that is still in development. More features and improvements are planned for the future.

# Libraries

GLFW and glad: GLFW is a library that provides a simple API for creating windows, contexts and surfaces, receiving input and events12. glad is a library that loads OpenGL functions at runtime.

cglm: cglm is a library that provides highly optimized 2D and 3D math operations for graphics, such as vector, matrix, quaternion, and affine transformations3.

stb_image: stb_image is a library that can load, decode and write images from file or memory. It supports most popular file formats, such as JPG, PNG, TGA, BMP, PSD, GIF, HDR, and PIC.
