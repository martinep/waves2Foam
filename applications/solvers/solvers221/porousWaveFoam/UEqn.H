    surfaceScalarField muEff
    (
        "muEff",
        twoPhaseProperties.muf()
      + fvc::interpolate(rho*turbulence->nut())
    );

    // Calculate and cache mu for the porous media
    volScalarField mu(twoPhaseProperties.mu());

    fvVectorMatrix UEqn
    (
        pZones.ddt(rho, U)
      + 1.0/pZones.porosity()*fvm::div(rhoPhi/(porosityFace), U)
      - fvm::laplacian(muEff/porosityFace, U)
      - (fvc::grad(U) & (1/pZones.porosity() * fvc::grad(muEff)))
    );

    UEqn.relax();

    pZones.addResistance(UEqn);

    if (pimple.momentumPredictor())
    {
        solve
        (
            UEqn
         ==
            fvc::reconstruct
            (
                (
                    fvc::interpolate(interface.sigmaK())*fvc::snGrad(alpha1)
                  - ghf*fvc::snGrad(rho)
                  - fvc::snGrad(p_rgh)
                )*mesh.magSf()
            )
        );
    }
