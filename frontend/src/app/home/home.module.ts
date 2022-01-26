import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { HomeRoutingModule } from './home-routing.module';
import { HomeComponent } from './home.component';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MatGridListModule } from '@angular/material/grid-list';
import { ToolCardComponent } from './tool-card/tool-card.component';


@NgModule({
  declarations: [
    HomeComponent,
    ToolCardComponent
  ],
    imports: [
        CommonModule,
        HomeRoutingModule,
        MatButtonModule,
        MatCardModule,
        MatGridListModule,
    ]
})
export class HomeModule { }
