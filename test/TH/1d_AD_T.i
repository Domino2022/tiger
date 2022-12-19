[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
  xmax = 1
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[UserObjects]
  [./rock_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-10'
  [../]
  [./supg]
    type = TigerSUPG
    effective_length = min
    supg_coeficient = optimal
  [../]
[]

[Materials]
  [./rock_g]
    type = TigerGeometryMaterial
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 1
    specific_density = 1
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_h]
    type = TigerHydraulicMaterialH
    pressure = pressure
    compressibility = 1.0e-4
    kf_uo = rock_uo
  [../]
  [./rock_t]
    type = TigerThermalMaterialT
    conductivity_type = isotropic
    lambda = 2
    specific_heat = 1
    has_supg = true
    supg_uo = supg
  [../]
[]

[BCs]
  [./front_p]
    type =  DirichletBC
    variable = pressure
    boundary = left
    value = 0
  [../]
  [./back_p]
    type =  DirichletBC
    variable = pressure
    boundary = right
    value = 1000
  [../]
  [./front_t]
   type =  DirichletBC
   variable = temperature
   boundary = left
   value = 0
  [../]
  [./back_t]
    type =  DirichletBC
    variable = temperature
    boundary = right
    value = 100
  [../]
[]

[ICs]
  [./temp]
    type = BoundingBoxIC
    variable = temperature
    inside = 0
    outside = 100
    boundary = right
    x1 = 0
    x2 = 0.999
    y1 = -1
    y2 = 1
  [../]
[]

[Variables]
  [./temperature]
    scaling = 5e-7
  [../]
  [./pressure]
  [../]
[]

[AuxVariables]
  [./vx]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pe]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./vx_ker]
    type = TigerDarcyVelocityH
    pressure = pressure
    variable =  vx
    component = x
    execute_on = timestep_end
  [../]
  [./pe_ker]
    type = MaterialRealAux
    property = 'thermal_peclet_number'
    variable = pe
  [../]
[]

[Kernels]
  [./T_diff]
    type = TigerThermalDiffusionKernelT
    variable = temperature
  [../]
  [./T_advect]
    type = TigerThermalAdvectionKernelT
    variable = temperature
    pressure = pressure
  [../]
  [./T_dt]
    type = TigerThermalTimeKernelT
    variable = temperature
  [../]
  [./H_diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
  [./H_dt]
    type = TigerHydraulicTimeKernelH
    variable = pressure
  [../]
[]

[Executioner]
  type = Transient
  dt = 10000
  end_time = 50000
  nl_abs_tol = 1e-14
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  #[./TimeStepper]
  #  type = SolutionTimeAdaptiveDT
  #  dt = 20
  #  percent_change = 0.5
  #[../]
[]

[Outputs]
  exodus = true
[]
