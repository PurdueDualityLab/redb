import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { ErrorRoutingModule } from './error-routing.module';
import { ErrorComponent } from './error.component';
import { MatButtonModule } from '@angular/material/button';


@NgModule({
  declarations: [
    ErrorComponent
  ],
    imports: [
        CommonModule,
        ErrorRoutingModule,
        MatButtonModule
    ]
})
export class ErrorModule { }
