import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';

const routes: Routes = [
  {
    path: 'query',
    loadChildren: () => import('./query/query.module').then(m => m.QueryModule)
  },
  {
    path: 'error',
    loadChildren: () => import('./error/error.module').then(m => m.ErrorModule)
  },
  {
    path: 'home',
    loadChildren: () => import('./home/home.module').then(m => m.HomeModule)
  },
  { path: 'analyze', loadChildren: () => import('./static-analysis/static-analysis.module').then(m => m.StaticAnalysisModule) },
  {
    path: '',
    pathMatch: 'full',
    redirectTo: '/home'
  },
  { path: 'rewrite', loadChildren: () => import('./rewrite/rewrite.module').then(m => m.RewriteModule) },
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
