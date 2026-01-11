var oX, oY, oZ;
var X, Y, Z;
// Set up the scene, camera, and renderer
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

const container = document.querySelector('.con');
const renderer = new THREE.WebGLRenderer();
renderer.setSize(container.clientWidth, container.clientHeight);
container.appendChild(renderer.domElement);

// Create a custom material for the axes
const axesMaterials = [
  new THREE.MeshBasicMaterial({ color: 0xff0000 }), // Red for x-axis
  new THREE.MeshBasicMaterial({ color: 0x00ff00 }), // Green for y-axis
  new THREE.MeshBasicMaterial({ color: 0x0000ff })  // Blue for z-axis
];

// Create a custom AxesHelper class
class CustomAxesHelper extends THREE.Object3D {
  constructor(size, thickness) {
    super();

    // Create the x-axis
    const xAxisGeometry = new THREE.CylinderGeometry(thickness, thickness, size, 32);
    const xAxis = new THREE.Mesh(xAxisGeometry, axesMaterials[0]);
    xAxis.rotation.z = Math.PI / 2; // Rotate to align with x-axis
    xAxis.position.x = 0; // Center at origin
    this.add(xAxis);

    // Create the y-axis
    const yAxisGeometry = new THREE.CylinderGeometry(thickness, thickness, size, 32);
    const yAxis = new THREE.Mesh(yAxisGeometry, axesMaterials[1]);
    yAxis.position.y = 0; // Center at origin
    this.add(yAxis);

    // Create the z-axis
    const zAxisGeometry = new THREE.CylinderGeometry(thickness, thickness, size, 32);
    const zAxis = new THREE.Mesh(zAxisGeometry, axesMaterials[2]);
    zAxis.rotation.x = Math.PI / 2; // Rotate to align with z-axis
    zAxis.position.z = 0; // Center at origin
    this.add(zAxis);
  }
}

// Add the custom axes helper to the scene
const customAxesHelper = new CustomAxesHelper(4, 0.1); // 4 is the length, 0.1 is the thickness
scene.add(customAxesHelper);

// Animation loop
function animate() {
  X = oX * (Math.PI / 180);
  Y = oY * (Math.PI / 180);
  Z = oZ * (Math.PI / 180);
  requestAnimationFrame(animate);

  // Rotate the custom axes helper around the y-axis
  customAxesHelper.rotation.x = X;
  customAxesHelper.rotation.y = Z;
  customAxesHelper.rotation.z = Y;

  // Render the scene
  renderer.render(scene, camera);
}

// Initialize the camera position
camera.position.z = 10;

// Handle window resize
window.addEventListener('resize', () => {
  camera.aspect = container.clientWidth / container.clientHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(container.clientWidth, container.clientHeight);
});

// Start the animation loop
animate();