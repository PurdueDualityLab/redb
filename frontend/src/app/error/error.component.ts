import { Component, OnInit } from '@angular/core';
import { ErrorService } from './error.service';
import { Subject } from 'rxjs';
import { ErrorState } from './error-state';

@Component({
  selector: 'app-error',
  templateUrl: './error.component.html',
  styleUrls: ['./error.component.scss']
})
export class ErrorComponent implements OnInit {

  constructor(private readonly errorService: ErrorService) { }

  ngOnInit(): void {
  }

  get errorState$(): Subject<ErrorState> {
    return this.errorService.errorState$;
  }
}
