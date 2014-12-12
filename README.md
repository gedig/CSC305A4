CSC305A4
========

Ray Tracer for CSC305 using QT.

UI Explanation:
	Open Image allows you to select an image to display in the viewer.
	Open Scene is used to select a .txt or .scn file to specify the objects in the scene.
		This will be explained in further detail below.
	Make Image gives the command to render the image with the last scene data provided.
	Save Image exports the rendered image to png format.


Scene File Format:
	Data for the scene can be saved in a .txt or a .scn file. The format is provided below:
	# Lines preceded by '#' are ignored by the interpreter, and can be used as comments.
	camera-z: float
	Only one camera z position can be defined. The camera x and y are always set to half of the render view.
	point-light: intensity:float, x:float, y:float, z:float
	Multiple point lights can be defined.
	sphere: radius:float, x: float, y:float, z:float
	Multiple spheres can be defined.
	
	An example file would look like the following:
	# This is a comment.
	camera-z: -100
	point-light: 50, 50, 300, 50
	sphere: 140, 300, 250, 65
	sphere: 40, 450, 100, 50

        A complete example can be found in scene.txt


Included Features:
	Complete lighting model; Ambient, Lambertian Diffuse, and Blinn-Phong Specular.
	Scenes defined in external files, granting the ability to modify the scene without
		recompiling or even rerunning the application.
        Ability to define materials and apply them to various objects and surfaces.
	Ability to export images to png


Data Structures Used:
	Material:
		A class that defines the various information necessary for calculations in the light model.
		It includes QColor objects representing the ambient, diffuse, and specular coefficients of the 
			material, as well as a phongExponent to be used for the specular.
	PointLight:
		A pointlight is simply a QVector3D representing its position and a QColor object 
			representing its intensity and colour.
	Sphere:
		A sphere is a QVector3D representing the position, a float radius, and a material.
		
	In the application, I have two lists for Spheres and Pointlights, and a hashmap of 
	Strings and materials so they can be referenced by a name.