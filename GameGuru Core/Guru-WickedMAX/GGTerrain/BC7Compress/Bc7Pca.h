#pragma once

#include "pch.h"
#include "Bc7Core.h"

#if defined(OPTION_PCA)

int PrincipalComponentAnalysis3(const Area& area);
int PrincipalComponentAnalysis4(const Area& area);

void InitPCA() noexcept;

#endif
