[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 10
  nz = 2
  ymax = 0
  ymin = -1
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Modules]
  [./TensorMechanics]
    [./Master]
      [./all]
      add_variables = true
      strain = SMALL
      incremental = true
      additional_generate_output = stress_yy
      [../]
    [../]
  [../]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
      viscosity = 0.001
    [../]
  [../]
[]

[UserObjects]
  [./rock_uo]
    type = TigerPermeabilityVar
    permeability_type = isotropic
    k0 = '1.0e-12'
    n0 = 0.2
  [../]
[]

[Variables]
  [./pressure]
  [../]
[]

[Kernels]
  [./hm]
    type = TigerHydroMechanicsKernelHM
    variable = pressure
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./hm_time]
    type = TigerHydraulicTimeKernelH
    variable = pressure
  [../]
  [./poro_x]
    type = PoroMechanicsCoupling
    variable = disp_x
    porepressure = pressure
    component = 0
  [../]
  [./poro_y]
    type = PoroMechanicsCoupling
    variable = disp_y
    porepressure = pressure
    component = 1
  [../]
  [./poro_z]
    type = PoroMechanicsCoupling
    variable = disp_z
    porepressure = pressure
    component = 2
  [../]
[]

[BCs]
  [./no_x]
    type = DirichletBC
    variable = disp_x
    boundary = bottom
    value = 0.0
  [../]
  [./no_y1]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./no_y2]
    type = FunctionNeumannBC
    variable = disp_y
    boundary = top
    function = 'if(x>0.5&z>0.5,if(t>10,if(t<40,-5e5*(t-10)/30,-5e5),0),0)'
  [../]
  [./no_z]
    type = DirichletBC
    variable = disp_z
    boundary = bottom
    value = 0.0
  [../]
  [./pressure]
    type =  DirichletBC
    variable = pressure
    boundary = top
    value = 0
  [../]
[]

[Materials]
  [./Elasticity_tensor]
    type = ComputeElasticityTensor
    fill_method = symmetric_isotropic_E_nu
    C_ijkl = '0.5e8 0'
  [../]
  [./stress]
    type = ComputeFiniteStrainElasticStress
  [../]
  [./rock_g]
    type = TigerGeometryMaterial
    gravity = '0 0 0'
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 0.2
    specific_density = 2500
    porosity_evolution = true
    output_properties = 'porosity'
    outputs = exodus
  [../]
  [./rock_m]
    type = TigerMechanicsMaterialM
    incremental = true
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_h]
    type = TigerHydraulicMaterialH
    pressure = pressure
    compressibility = 1.0e-9
    kf_uo = rock_uo
    output_properties = 'permeability_by_viscosity'
    outputs = exodus
  [../]
[]

[Preconditioning]
  [./p1]
    type = SMP
    full = true
    petsc_options_iname = '-snes_type -snes_linesearch_type'
    petsc_options_value = 'newtonls   basic               '
  [../]
[]

[Executioner]
  type = Transient
  end_time = 100
  dt = 10
  nl_abs_tol = 1e-10
  l_max_its = 20
  automatic_scaling = true
  compute_scaling_once = false
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]
